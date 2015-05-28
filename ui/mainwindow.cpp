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
#include "ui_mainwindow.h"

#include <elf/elffileset.h>

#include <checks/structurepackingcheck.h>
#include <checks/virtualdtorcheck.h>

#include <elfmodel/elfmodel.h>
#include <navigator/codenavigator.h>

#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QStatusBar>

#include <elf.h>

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent), ui(new Ui::MainWindow), m_elfModel(new ElfModel(this))
{
    ui->setupUi(this);

    ui->elfStructureView->setModel(m_elfModel);
    ui->sizeTreeMapView->setModel(m_elfModel);

    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::tabChanged);

    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::fileOpen);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionReopenPreviousFile, &QAction::triggered, this, &MainWindow::reloadFileOnStartup);

    ui->menuSettings->addAction(CodeNavigator::configMenu(this));

    // ### temporary
    connect(ui->actionCheckStructurePacking, &QAction::triggered, this, [this]() {
        StructurePackingCheck checker;
        checker.setElfFileSet(m_fileSet.get());
        checker.checkAll(m_fileSet->file(0)->dwarfInfo());
    });
    connect(ui->actionVirtualDestructors, &QAction::triggered, this, [this]() {
        VirtualDtorCheck checker;
        checker.findImplicitVirtualDtors(m_fileSet.get());
    });

    restoreSettings();
    tabChanged();
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("currentTab", ui->tabWidget->currentIndex());
    settings.endGroup();
    QMainWindow::closeEvent(event);
}

void MainWindow::fileOpen()
{
    const QFileInfo fi(m_currentFileName);

    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open ELF Object"), fi.path());
    if (fileName.isEmpty())
        return;

    m_currentFileName = fileName;
    loadFile(fileName);
}

void MainWindow::reloadFileOnStartup()
{
    QSettings settings;
    settings.setValue("Settings/ReloadPreviousFile", ui->actionReopenPreviousFile->isChecked());
}

void MainWindow::restoreSettings()
{
    QSettings settings;
    ui->sizeTreeMapView->restoreSettings();
    ui->actionReopenPreviousFile->setChecked(settings.value("Settings/ReloadPreviousFile", true).toBool());
    if (ui->actionReopenPreviousFile->isChecked()) {
        const auto fileName = settings.value("Recent/PreviousFile").toString();
        const QFileInfo fi(fileName);
        if (!fi.isReadable() || !fi.isFile())
            return;
        m_currentFileName = std::move(fileName);
        loadFile(m_currentFileName);
    }

    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    ui->tabWidget->setCurrentIndex(settings.value("currentTab", 0).toInt());
    settings.endGroup();
}

void MainWindow::loadFile(const QString& fileName)
{
    if (fileName.isEmpty() || windowFilePath() == fileName)
        return;
    setWindowFilePath(fileName);

    m_fileSet.reset(new ElfFileSet(this));
    m_fileSet->addFile(fileName);

    if (m_fileSet->size() == 0) {
        QMessageBox::critical(this, tr("Failed to load ELF file"), tr("Could not load %1.").arg(fileName));

        m_fileSet.reset();

        statusBar()->showMessage(tr("Failed to load %1.").arg(fileName));
    } else {
        m_fileSet->topologicalSort();

        QSettings settings;
        settings.setValue("Recent/PreviousFile", fileName);

        statusBar()->showMessage(tr("Loaded %1.").arg(fileName));
    }

    m_elfModel->setFileSet(m_fileSet.get());
    ui->dependencyView->setFileSet(m_fileSet.get());
    ui->loadTimeView->setFileSet(m_fileSet.get());
    ui->typeView->setFileSet(m_fileSet.get());
}

void MainWindow::tabChanged()
{
    ui->menuView->clear();
    ui->menuView->addActions(ui->tabWidget->currentWidget()->actions());
    ui->menuView->setEnabled(!ui->menuView->isEmpty());
}
