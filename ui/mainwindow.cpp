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

#include <checks/ldbenchmark.h>
#include <checks/structurepackingcheck.h>
#include <checks/virtualdtorcheck.h>

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
    ui->sizeTreeMapView->setModel(m_elfModel);

    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::tabChanged);

    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::fileOpen);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionReopenPreviousFile, &QAction::triggered, this, &MainWindow::reloadFileOnStartup);

    // ### temporary
    connect(ui->actionCheckStructurePacking, &QAction::triggered, this, [this]() {
        StructurePackingCheck checker;
        checker.setElfFileSet(m_fileSet);
        checker.checkAll(m_fileSet->file(0)->dwarfInfo());
    });
    connect(ui->actionLDBenchmark, &QAction::triggered, this, [this]() {
        LDBenchmark benchmark;
        benchmark.measureFileSet(m_fileSet);
    });
    connect(ui->actionVirtualDestructors, &QAction::triggered, this, [this]() {
        VirtualDtorCheck checker;
        checker.findImplicitVirtualDtors(m_fileSet);
    });

    connect(ui->elfStructureView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::selectionChanged);
    connect(ui->elfStructureSearchLine, &QLineEdit::textChanged, this, [filter](const QString &text) { filter->setFilterFixedString(text); });

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
    settings.endGroup();
    QMainWindow::closeEvent(event);
}

void MainWindow::fileOpen()
{
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open ELF Object"));
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
    settings.endGroup();
}

void MainWindow::loadFile(const QString& fileName)
{
    if (fileName.isEmpty() || windowFilePath() == fileName)
        return;
    setWindowFilePath(fileName);

    delete m_fileSet;
    m_fileSet = new ElfFileSet(this);
    m_fileSet->addFile(fileName);
    m_fileSet->topologicalSort();
    m_elfModel->setFileSet(m_fileSet);
    ui->elfStructureView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->dependencyView->setFileSet(m_fileSet);

    QSettings settings;
    settings.setValue("Recent/PreviousFile", fileName);

    statusBar()->showMessage(tr("Loaded %1.").arg(fileName));
}

void MainWindow::tabChanged()
{
    ui->menuView->clear();
    ui->menuView->addActions(ui->tabWidget->currentWidget()->actions());
    ui->menuView->setEnabled(!ui->menuView->isEmpty());
}

void MainWindow::selectionChanged(const QItemSelection& selection)
{
    if (selection.isEmpty())
        ui->elfDetailView->clear();

    const QModelIndex index = selection.first().topLeft();
    ui->elfDetailView->setHtml(index.data(ElfModel::DetailRole).toString());
}
