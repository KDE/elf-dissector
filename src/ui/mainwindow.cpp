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
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <elf/elffileset.h>

#include <checks/structurepackingcheck.h>

#include <elfmodel/elfmodel.h>
#include <navigator/codenavigator.h>

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QStatusBar>
#include <QStandardItemModel>

#include <elf.h>

static void addView(QStandardItemModel *model, const QString& iconName, const QString& title)
{
    auto icon = QIcon::fromTheme(iconName);
    if (icon.isNull())
        icon = QIcon::fromTheme(QStringLiteral("dialog-information")); // fallback
    model->appendRow(new QStandardItem(icon, title));
}

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent), ui(new Ui::MainWindow), m_elfModel(new ElfModel(this))
{
    ui->setupUi(this);

    ui->elfStructureView->setModel(m_elfModel);
    ui->sizeTreeMapView->setModel(m_elfModel);

    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, &MainWindow::currentViewChanged);

    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::fileOpen);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionReopenPreviousFile, &QAction::triggered, this, &MainWindow::reloadFileOnStartup);

    auto viewModel = new QStandardItemModel(this);
    addView(viewModel, QStringLiteral("document-preview"), QStringLiteral("ELF Structure"));
    addView(viewModel, QStringLiteral("table"), QStringLiteral("Size Tree Map"));
    addView(viewModel, QStringLiteral("view-list-tree"), QStringLiteral("Dependencies"));
    addView(viewModel, QStringLiteral("code-class"), QStringLiteral("Data Types"));
    addView(viewModel, QStringLiteral("chronometer"), QStringLiteral("Performance"));
    addView(viewModel, QStringLiteral("dialog-warning"), QStringLiteral("Issues"));
    ui->viewSelector->setModel(viewModel);
    connect(ui->viewSelector, &SidePane::currentIndexChanged, this, [this](int index){
        ui->stackedWidget->setCurrentIndex(index);
    });

    ui->menuSettings->addAction(CodeNavigator::configMenu(this));

    restoreSettings();
    currentViewChanged();
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("MainWindow"));
    settings.setValue(QStringLiteral("geometry"), saveGeometry());
    settings.setValue(QStringLiteral("windowState"), saveState());
    settings.setValue(QStringLiteral("currentView"), ui->stackedWidget->currentIndex());
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
    settings.setValue(QStringLiteral("Settings/ReloadPreviousFile"), ui->actionReopenPreviousFile->isChecked());
}

void MainWindow::restoreSettings()
{
    QSettings settings;
    ui->sizeTreeMapView->restoreSettings();
    ui->actionReopenPreviousFile->setChecked(settings.value(QStringLiteral("Settings/ReloadPreviousFile"), false).toBool());
    if (ui->actionReopenPreviousFile->isChecked()) {
        auto fileName = settings.value(QStringLiteral("Recent/PreviousFile")).toString();
        const QFileInfo fi(fileName);
        if (!fi.isReadable() || !fi.isFile())
            return;
        m_currentFileName = std::move(fileName);
        loadFile(m_currentFileName);
    }

    settings.beginGroup(QStringLiteral("MainWindow"));
    restoreGeometry(settings.value(QStringLiteral("geometry")).toByteArray());
    restoreState(settings.value(QStringLiteral("windowState")).toByteArray());
    const auto currentViewIndex = settings.value(QStringLiteral("currentView"), 0).toInt();
    const auto currentIdx = ui->viewSelector->model()->index(currentViewIndex, 0);
    ui->viewSelector->selectionModel()->select(currentIdx, QItemSelectionModel::ClearAndSelect);
    ui->viewSelector->selectionModel()->setCurrentIndex(currentIdx, QItemSelectionModel::ClearAndSelect);
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
        settings.setValue(QStringLiteral("Recent/PreviousFile"), fileName);

        statusBar()->showMessage(tr("Loaded %1.").arg(fileName));
    }

    m_elfModel->setFileSet(m_fileSet.get());
    ui->dependencyView->setFileSet(m_fileSet.get());
    ui->loadTimeView->setFileSet(m_fileSet.get());
    ui->typeView->setFileSet(m_fileSet.get());
    ui->issuesView->setFileSet(m_fileSet.get());
}

void MainWindow::currentViewChanged()
{
    ui->menuView->clear();
    ui->menuView->addActions(ui->stackedWidget->currentWidget()->actions());
    ui->menuView->setEnabled(!ui->menuView->isEmpty());
}
