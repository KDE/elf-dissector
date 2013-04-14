#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <elf/elffile.h>
#include <elf/elfsymboltablesection.h>
#include <elf/elfstringtablesection.h>

#include <demangle/demangler.h>

#include <treemap/treemap.h>

#include <QApplication>
#include <QFileDialog>
#include <QSettings>
#include <QStatusBar>

#include <elf.h>

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::fileOpen);
    connect(ui->actionQuit, &QAction::triggered, &QCoreApplication::quit);
    connect(ui->actionHideDebugInformation, &QAction::triggered, this, &MainWindow::hideDebugInformation);

    restoreSettings();
}

MainWindow::~MainWindow()
{
}

void MainWindow::fileOpen()
{
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open ELF Object"));
    setWindowFilePath(fileName);
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

void MainWindow::restoreSettings()
{
    QSettings settings;
    ui->actionHideDebugInformation->setChecked(settings.value("View/HideDebugInfo", false).toBool());
    ui->actionReopenPreviousFile->setChecked(settings.value("Settings/ReloadPreviousFile", true).toBool());
    if (ui->actionReopenPreviousFile->isChecked()) {
        m_currentFileName = settings.value("Recent/PreviousFile").toString();
        if (!m_currentFileName.isEmpty())
            loadFile(m_currentFileName);
    }
}

void MainWindow::loadFile(const QString& fileName)
{
    // TODO all temporary, still needs a proper model!

    delete m_treeMap; // TODO: really needed? deletes items as well?

    ElfFile file(fileName);

    TreeMapItem *baseItem = new TreeMapItem;
    baseItem->setText(0, file.displayName());
    baseItem->setSum(file.size());

    m_treeMap = new TreeMapWidget(baseItem);
//     m_treeMap->setMinimalArea(200);
//     m_treeMap->setVisibleWidth(10, false);
    // looks weird, but this forces m_treeMap->_attrs to be resided correctly for text to be drawn
    m_treeMap->setFieldForced(1, true);
    m_treeMap->setFieldForced(1, false);
    ui->tab_2->layout()->addWidget(m_treeMap);

    struct SymbolNode {
        TreeMapItem *item;
        QHash<QByteArray, SymbolNode*> children;
    };

    QVector<SymbolNode*> sectionItems;
    sectionItems.resize(file.sectionHeaders().size());

    for (const ElfSectionHeader::Ptr &shdr : file.sectionHeaders()) {
        if (ui->actionHideDebugInformation->isChecked() && shdr->isDebugInformation()) {
            baseItem->setSum(baseItem->sum() - shdr->size());
            continue;
        }
        auto item = new TreeMapItem(baseItem, shdr->size(), shdr->name(), QString::number(shdr->size()));
        item->setSum(shdr->size());
        sectionItems[shdr->sectionIndex()] = new SymbolNode;
        sectionItems[shdr->sectionIndex()]->item = item;
    }

    Demangler demangler;

    for (const ElfSectionHeader::Ptr &shdr : file.sectionHeaders()) {
        if (ui->actionHideDebugInformation->isChecked() && shdr->isDebugInformation())
            continue;
        if (shdr->type() == SHT_SYMTAB) {
            auto symtab = file.section<ElfSymbolTableSection>(shdr->sectionIndex());
            for (unsigned int j = 0; j < (shdr->size() / shdr->entrySize()); ++j) {
                // TODO make these shared pointers and keep them in the section object
                ElfSymbolTableSection::ElfSymbolTableEntry *entry = symtab->entry(j);
                if (entry->size() < 128 || !sectionItems.at(entry->sectionIndex()))
                    continue;
                SymbolNode *parentNode = sectionItems.at(entry->sectionIndex());
                const QVector<QByteArray> demangledNames = demangler.demangle(symtab->linkedSection<ElfStringTableSection>()->string(entry->name()));
                for (const QByteArray &demangledName : demangledNames) {
                    SymbolNode *node = parentNode->children.value(demangledName);
                    if (!node) {
                        node = new SymbolNode;
                        node->item = new TreeMapItem(parentNode->item);
                        node->item->setField(0, demangledName);
                        parentNode->children.insert(demangledName, node);
                    }
                    node->item->setSum(node->item->sum() + entry->size());
                    node->item->setValue(node->item->sum());
                    node->item->setField(1, QString::number(node->item->sum()));
                    parentNode = node;
                }
                delete entry;
            }
        }

    }

    QSettings settings;
    settings.setValue("Recent/PreviousFile", fileName);

    statusBar()->showMessage(tr("Loaded %1.").arg(fileName));
}
