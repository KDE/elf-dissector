#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <elf/elffile.h>
#include <treemap/treemap.h>

#include <QApplication>
#include <QFileDialog>

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
    if (fileName.isEmpty())
        return;

    ElfFile file(fileName);
    file.parse();

    TreeMapItem *baseItem = new TreeMapItem;
    baseItem->setText(0, "myfile.so");
    baseItem->setSum(1.0);

    m_treeMap = new TreeMapWidget(baseItem);
    // looks weird, but this forces m_treeMap->_attrs to be resided correctly for text to be drawn
    m_treeMap->setFieldForced(1, true);
    m_treeMap->setFieldForced(1, false);
    ui->tab_2->layout()->addWidget(m_treeMap);

    auto item = new TreeMapItem(baseItem, 0.5, "hello1", "hello2");
    item->setBackColor(Qt::red);
    item = new TreeMapItem(baseItem, 0.4, "bla", "blub", "blab", "lalala");
    item->setBackColor(Qt::green);
}
