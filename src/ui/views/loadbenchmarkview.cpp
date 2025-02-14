/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "loadbenchmarkview.h"
#include "ui_loadbenchmarkview.h"

#include <checks/ldbenchmark.h>
#include <plotter/gnuplotter.h>
#include <loadbenchmarkmodel/loadbenchmarkmodel.h>

#include <QSortFilterProxyModel>

using namespace Qt::Literals;

LoadBenchmarkView::LoadBenchmarkView(QWidget* parent):
    QWidget(parent),
    ui(new Ui::LoadBenchmarkView),
    m_model(new LoadBenchmarkModel(this))
{
    ui->setupUi(this);
    ui->runButton->setDefaultAction(ui->actionRunBenchmark);
    auto proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(m_model);
    ui->dataView->setModel(proxy);

    ui->actionRunBenchmark->setEnabled(Gnuplotter::hasGnuplot());
    connect(ui->actionRunBenchmark, &QAction::triggered, this, &LoadBenchmarkView::runBenchmark);

    addActions({ ui->actionRunBenchmark });
}

LoadBenchmarkView::~LoadBenchmarkView() = default;

void LoadBenchmarkView::setFileSet(ElfFileSet* fileSet)
{
    m_fileSet = fileSet;
}

void LoadBenchmarkView::runBenchmark()
{
    if (!m_fileSet)
        return;

    m_benchmark = std::make_shared<LDBenchmark>();
    m_benchmark->measureFileSet(m_fileSet);

    Gnuplotter plotter;
    plotter.setSize(ui->plotter->size());
    plotter.setTemplate(QStringLiteral(":/ldbenchmark.gnuplot"));

    m_benchmark->writeCSV(plotter.workingDir() + "/ldbenchmark.csv"_L1);
    ui->plotter->setPlotter(std::move(plotter));

    m_model->setBenchmark(m_benchmark);
}
