#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <elf/elffile.h>
#include <elf/elfsymboltablesection.h>
#include <elf/elfstringtablesection.h>

#include <demangle/demangler.h>

#include <treemap/treemap.h>

#include <QApplication>
#include <QFileDialog>

#include <elf.h>

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::fileOpen);
    connect(ui->actionQuit, &QAction::triggered, &QCoreApplication::quit);
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

    ElfFile file(fileName);
    file.parse();

    TreeMapItem *baseItem = new TreeMapItem;
    baseItem->setText(0, file.displayName());
    baseItem->setSum(file.size());

    m_treeMap = new TreeMapWidget(baseItem);
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
        auto item = new TreeMapItem(baseItem, shdr->size(), file.stringTableEntry(shdr->name()), QString::number(shdr->size()));
        item->setSum(shdr->size());
        sectionItems[shdr->sectionIndex()] = new SymbolNode;
        sectionItems[shdr->sectionIndex()]->item = item;
    }

    Demangler demangler;

    for (const ElfSectionHeader::Ptr &shdr : file.sectionHeaders()) {
        if (shdr->type() == SHT_SYMTAB) {
            auto symtab = file.section<ElfSymbolTableSection>(shdr->sectionIndex());
            for (unsigned int j = 0; j < (shdr->size() / shdr->entrySize()); ++j) {
                // TODO make these shared pointers and keep them in the section object
                ElfSymbolTableSection::ElfSymbolTableEntry *entry = symtab->entry(j);
                if (entry->size() < 128)
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
}
