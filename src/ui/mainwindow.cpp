/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "config-elf-dissector.h"

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

static void addView(QStandardItemModel *model, const QString& iconName, const QString& title, int index)
{
    auto icon = QIcon::fromTheme(iconName);
    if (icon.isNull())
        icon = QIcon::fromTheme(QStringLiteral("dialog-information")); // fallback
    auto item = new QStandardItem(icon, title);
    item->setData(index, Qt::UserRole);
    model->appendRow(item);
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
    addView(viewModel, QStringLiteral("document-preview"), tr("ELF Structure"), 0);
    addView(viewModel, QStringLiteral("table"), tr("Size Tree Map"), 1);
    addView(viewModel, QStringLiteral("view-list-tree"), tr("Dependencies"), 2);
#if HAVE_DWARF
    addView(viewModel, QStringLiteral("code-class"), tr("Data Types"), 3);
#endif
    addView(viewModel, QStringLiteral("chronometer"), tr("Performance"), 4);
#if HAVE_DWARF
    addView(viewModel, QStringLiteral("dialog-warning"), tr("Issues"), 5);
#endif
    ui->viewSelector->setModel(viewModel);
    connect(ui->viewSelector, &SidePane::currentIndexChanged, this, [this, viewModel](int index){
        const auto viewIdx = viewModel->index(index, 0).data(Qt::UserRole).toInt();
        ui->stackedWidget->setCurrentIndex(viewIdx);
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
