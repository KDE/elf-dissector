/*
    Copyright (C) 2013-2014 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"
#include "colorizer.h"
#include "ui_mainwindow.h"

#include <elf/elffile.h>
#include <elf/elfsymboltablesection.h>
#include <elf/elfstringtablesection.h>
#include <elf/elffileset.h>

#include <demangle/demangler.h>

#include <checks/structurepackingcheck.h>

#include <model/elfmodel.h>

#include <treemap/treemap.h>

#include <kitemmodels/krecursivefilterproxymodel.h>

#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QSettings>
#include <QStatusBar>

#include <elf.h>

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent), ui(new Ui::MainWindow), m_elfModel(new ElfModel(this))
{
    ui->setupUi(this);

    auto *filter = new KRecursiveFilterProxyModel(this);
    filter->setSourceModel(m_elfModel);
    ui->elfStructureView->setModel(filter);

    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::fileOpen);
    connect(ui->actionQuit, &QAction::triggered, &QCoreApplication::quit);
    connect(ui->actionHideDebugInformation, &QAction::triggered, this, &MainWindow::hideDebugInformation);
    connect(ui->actionColorizeSections, &QAction::triggered, this, &MainWindow::colorizationChanged);
    connect(ui->actionColorizeSymbols, &QAction::triggered, this, &MainWindow::colorizationChanged);
    connect(ui->actionReopenPreviousFile, &QAction::triggered, this, &MainWindow::reloadFileOnStartup);

    // ### temporary
    connect(ui->actionCheckStructurePacking, &QAction::triggered, this, [this]() {
        StructurePackingCheck checker(m_fileSet);
        checker.checkAll(m_fileSet->file(0)->dwarfInfo());
    });

    connect(ui->elfStructureView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::selectionChanged);
    connect(ui->elfStructureSearchLine, &QLineEdit::textChanged, this, [filter](const QString &text) { filter->setFilterFixedString(text); });

    restoreSettings();
}

MainWindow::~MainWindow()
{
}

void MainWindow::fileOpen()
{
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open ELF Object"));
    if (fileName.isEmpty())
        return;

    m_currentFileName = fileName;
    loadFile(fileName);
}

void MainWindow::hideDebugInformation()
{
    QSettings settings;
    settings.setValue("View/HideDebugInfo", ui->actionHideDebugInformation->isChecked());
    loadFile(m_currentFileName);
}

void MainWindow::reloadFileOnStartup()
{
    QSettings settings;
    settings.setValue("Settings/ReloadPreviousFile", ui->actionReopenPreviousFile->isChecked());
}

void MainWindow::colorizationChanged()
{
    QSettings settings;
    settings.setValue("View/ColorizeSections", ui->actionColorizeSections->isChecked());
    settings.setValue("View/ColorizeSymbols", ui->actionColorizeSymbols->isChecked());
    loadFile(m_currentFileName);
}

void MainWindow::restoreSettings()
{
    QSettings settings;
    ui->actionHideDebugInformation->setChecked(settings.value("View/HideDebugInfo", false).toBool());
    ui->actionColorizeSections->setChecked(settings.value("View/ColorizeSections", true).toBool());
    ui->actionColorizeSymbols->setChecked(settings.value("View/ColorizeSymbols", false).toBool());
    ui->actionReopenPreviousFile->setChecked(settings.value("Settings/ReloadPreviousFile", true).toBool());
    if (ui->actionReopenPreviousFile->isChecked()) {
        const auto fileName = settings.value("Recent/PreviousFile").toString();
        const QFileInfo fi(fileName);
        if (!fi.isReadable() || !fi.isFile())
            return;
        m_currentFileName = std::move(fileName);
        loadFile(m_currentFileName);
    }
}

void MainWindow::treeMapContextMenu(const QPoint& pos)
{
    QMenu menu;
    m_treeMap->addSplitDirectionItems(&menu);
    menu.exec(mapToGlobal(pos));

    QSettings settings;
    settings.setValue("TreeMap/SplitMode", m_treeMap->splitModeString());
}

void MainWindow::loadFile(const QString& fileName)
{
    if (fileName.isEmpty())
        return;
    setWindowFilePath(fileName);

    // TODO all temporary, still needs a proper model!

    delete m_treeMap; // TODO: really needed? deletes items as well?

    delete m_fileSet;
    m_fileSet = new ElfFileSet(this);
    m_fileSet->addFile(fileName);
    m_elfModel->setFileSet(m_fileSet);
    ui->elfStructureView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);


    ElfFile::Ptr file = m_fileSet->file(0);
    TreeMapItem *baseItem = new TreeMapItem;
    baseItem->setText(0, file->displayName());
    baseItem->setSum(file->size());

    m_treeMap = new TreeMapWidget(baseItem);
    m_treeMap->setBorderWidth(3);
//     m_treeMap->setMinimalArea(200);
//     m_treeMap->setVisibleWidth(10, false);
    // looks weird, but this forces m_treeMap->_attrs to be resided correctly for text to be drawn
    m_treeMap->setFieldForced(1, true);
    m_treeMap->setFieldForced(1, false);
    ui->tab_2->layout()->addWidget(m_treeMap);
    m_treeMap->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_treeMap, &TreeMapWidget::customContextMenuRequested, this, &MainWindow::treeMapContextMenu);

    QSettings settings;
    m_treeMap->setSplitMode(settings.value("TreeMap/SplitMode", "Bisection").toString());

    struct SymbolNode {
        TreeMapItem *item;
        QHash<QByteArray, SymbolNode*> children;
    };

    QVector<SymbolNode*> sectionItems;
    sectionItems.resize(file->sectionHeaders().size());

    Colorizer sectionColorizer(96, 255);
    for (const ElfSectionHeader::Ptr &shdr : file->sectionHeaders()) {
        if (ui->actionHideDebugInformation->isChecked() && shdr->isDebugInformation()) {
            baseItem->setSum(baseItem->sum() - shdr->size());
            continue;
        }
        auto item = new TreeMapItem(baseItem, shdr->size(), shdr->name(), QString::number(shdr->size()));
        item->setSum(shdr->size());
        item->setSorting(-2, true); // sort by value
        if (ui->actionColorizeSections->isChecked())
            item->setBackColor(sectionColorizer.nextColor());
        sectionItems[shdr->sectionIndex()] = new SymbolNode;
        sectionItems[shdr->sectionIndex()]->item = item;
    }

    Colorizer symbolColorizer;
    Demangler demangler;

    for (const ElfSectionHeader::Ptr &shdr : file->sectionHeaders()) {
        if (ui->actionHideDebugInformation->isChecked() && shdr->isDebugInformation())
            continue;
        if (shdr->type() == SHT_SYMTAB) {
            auto symtab = file->section<ElfSymbolTableSection>(shdr->sectionIndex());
            for (unsigned int j = 0; j < (shdr->size() / shdr->entrySize()); ++j) {
                ElfSymbolTableEntry::Ptr entry = symtab->entry(j);
                if (entry->size() == 0 || !sectionItems.at(entry->sectionIndex()))
                    continue;
                SymbolNode *parentNode = sectionItems.at(entry->sectionIndex());
                const QVector<QByteArray> demangledNames = demangler.demangle(entry->name());
                for (const QByteArray &demangledName : demangledNames) {
                    SymbolNode *node = parentNode->children.value(demangledName);
                    if (!node) {
                        node = new SymbolNode;
                        node->item = new TreeMapItem(parentNode->item);
                        node->item->setField(0, demangledName);
                        if (ui->actionColorizeSymbols->isChecked() && parentNode->item->parent() == baseItem) {
                            node->item->setBackColor(symbolColorizer.nextColor());
                        } else {
                            node->item->setBackColor(parentNode->item->backColor());
                        }
                        parentNode->children.insert(demangledName, node);
                    }
                    node->item->setSum(node->item->sum() + entry->size());
                    node->item->setValue(node->item->sum());
                    node->item->setField(1, QString::number(node->item->sum()));
                    parentNode = node;
                }
            }
        }
    }

    settings.setValue("Recent/PreviousFile", fileName);

    statusBar()->showMessage(tr("Loaded %1.").arg(fileName));
}

void MainWindow::selectionChanged(const QItemSelection& selection)
{
    if (selection.isEmpty())
        ui->elfDetailView->clear();

    const QModelIndex index = selection.first().topLeft();
    ui->elfDetailView->setHtml(index.data(ElfModel::DetailRole).toString());
}
