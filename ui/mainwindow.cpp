#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <elf/elffile.h>

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
}
