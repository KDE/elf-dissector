#include "mainwindow.h"
#include "colorizer.h"
#include "ui_mainwindow.h"

#include <elf/elffile.h>
#include <elf/elfsymboltablesection.h>
#include <elf/elfstringtablesection.h>
#include <elf/elffileset.h>

#include <demangle/demangler.h>

#include <model/elfmodel.h>

#include <treemap/treemap.h>

#include <QApplication>
#include <QFileDialog>
#include <QSettings>
#include <QStatusBar>

#include <elf.h>

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent), ui(new Ui::MainWindow), m_elfModel(new ElfModel(this))
{
    ui->setupUi(this);
    ui->elfStructureView->setModel(m_elfModel);

    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::fileOpen);
    connect(ui->actionQuit, &QAction::triggered, &QCoreApplication::quit);
    connect(ui->actionHideDebugInformation, &QAction::triggered, this, &MainWindow::hideDebugInformation);
    connect(ui->actionColorizeSections, &QAction::triggered, this, &MainWindow::colorizationChanged);
    connect(ui->actionColorizeSymbols, &QAction::triggered, this, &MainWindow::colorizationChanged);

    connect(ui->elfStructureView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::selectionChanged);

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
        m_currentFileName = settings.value("Recent/PreviousFile").toString();
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
    Colorizer colorizer;

    for (const ElfSectionHeader::Ptr &shdr : file->sectionHeaders()) {
        if (ui->actionHideDebugInformation->isChecked() && shdr->isDebugInformation()) {
            baseItem->setSum(baseItem->sum() - shdr->size());
            continue;
        }
        auto item = new TreeMapItem(baseItem, shdr->size(), shdr->name(), QString::number(shdr->size()));
        item->setSum(shdr->size());
        item->setSorting(-2, true); // sort by value
        if (ui->actionColorizeSections->isChecked())
            item->setBackColor(colorizer.nextColor());
        sectionItems[shdr->sectionIndex()] = new SymbolNode;
        sectionItems[shdr->sectionIndex()]->item = item;
    }

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
                            node->item->setBackColor(colorizer.nextColor());
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
