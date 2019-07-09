/*
    Copyright (C) 2015 Volker Krause <vkrause@kde.org>

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

#include "dependencyview.h"
#include "ui_dependencyview.h"
#include <dependencymodel/dependencymodel.h>
#include <dependencymodel/usedsymbolmodel.h>
#include <dependencymodel/unuseddependenciesmodel.h>
#include <dependencymodel/filelistmodel.h>
#include <dependencymodel/fileusermodel.h>

#include <elf/elffile.h>
#include <elf/elffileset.h>
#include <optimizers/dependencysorter.h>

#include <QDebug>
#include <QSortFilterProxyModel>

DependencyView::DependencyView(QWidget* parent):
    QWidget(parent),
    ui(new Ui::DependencyView),
    m_dependencyModel(new DependencyModel(this)),
    m_symbolModel(new UsedSymbolModel(this)),
    m_unusedDependenciesModel(new UnusedDependenciesModel(this)),
    m_fileListModel(new FileListModel(this)),
    m_fileUserModel(new FileUserModel(this)),
    m_inverseSymbolModel(new UsedSymbolModel(this))
{
    ui->setupUi(this);

    // top down dependencies tab
    ui->dependencyView->setModel(m_dependencyModel);
    ui->dependencyView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    connect(ui->dependencySearchLine, &QLineEdit::textChanged, this, &DependencyView::search);
    connect(ui->dependencyView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DependencyView::dependencySelected);

    auto proxy = new QSortFilterProxyModel(this);
    proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxy->setSourceModel(m_symbolModel);
    ui->symbolView->setModel(proxy);
    connect(ui->symbolSearchLine, &QLineEdit::textChanged, proxy, &QSortFilterProxyModel::setFilterFixedString);

    // inverse dependencies tab
    proxy = new QSortFilterProxyModel(this);
    proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxy->setSourceModel(m_fileListModel);
    ui->inverseFileList->setModel(proxy);
    connect(ui->inverseFileListSearchLine, &QLineEdit::textChanged, proxy, &QSortFilterProxyModel::setFilterFixedString);
    connect(ui->inverseFileList->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DependencyView::inverseFileSelected);

    proxy = new QSortFilterProxyModel(this);
    proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxy->setSourceModel(m_fileUserModel);
    ui->inverseDependencyList->setModel(proxy);
    connect(ui->inverseDependencyListSearchLine, &QLineEdit::textChanged, proxy, &QSortFilterProxyModel::setFilterFixedString);
    connect(ui->inverseDependencyList->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DependencyView::inverseUserSelected);

    proxy = new QSortFilterProxyModel(this);
    proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxy->setSourceModel(m_inverseSymbolModel);
    ui->inverseSymbolList->setModel(proxy);
    ui->inverseSymbolList->header()->setSortIndicator(0, Qt::AscendingOrder);
    connect(ui->inverseSymbolListSearchLine, &QLineEdit::textChanged, proxy, &QSortFilterProxyModel::setFilterFixedString);

    // unused dependencies tab
    proxy = new QSortFilterProxyModel(this);
    proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxy->setFilterKeyColumn(-1);
    proxy->setSourceModel(m_unusedDependenciesModel);
    ui->unusedDependencyView->setModel(proxy);
    ui->unusedDependencyView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    connect(ui->unusedSearchLine, &QLineEdit::textChanged, proxy, &QSortFilterProxyModel::setFilterFixedString);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [this]{
        if (ui->tabWidget->currentWidget() == ui->unusedDependenciesTab)
            m_unusedDependenciesModel->findUnusedDependencies();
    });

    addAction(ui->actionOptimizeDependencyOrder);
    connect(ui->actionOptimizeDependencyOrder, &QAction::triggered, this, [this]() {
        const auto selection = ui->dependencyView->selectionModel()->selectedRows();
        if (selection.isEmpty())
            return;
        const auto idx = selection.first();
        const auto f = idx.data(DependencyModel::ProviderFileRole).value<ElfFile*>();
        if (f) {
            qDebug() << f->fileName();
            ElfFileSet set;
            set.addFile(f->fileName());
            DependencySorter sorter;
            sorter.sortDtNeeded(&set);

            // TODO reload file set!
        }
    });
}

DependencyView::~DependencyView() = default;

void DependencyView::setFileSet(ElfFileSet* fileSet)
{
    m_dependencyModel->setFileSet(fileSet);
    if (m_dependencyModel->rowCount() > 0)
        ui->dependencyView->expand(m_dependencyModel->index(0, 0));
    m_fileListModel->setFileSet(fileSet);
    m_unusedDependenciesModel->setFileSet(fileSet);
    if (ui->tabWidget->currentWidget() == ui->unusedDependenciesTab)
        m_unusedDependenciesModel->findUnusedDependencies();
}

void DependencyView::search(const QString& text)
{
    if (text.isEmpty())
        return;

    const auto result = m_dependencyModel->match(m_dependencyModel->index(0, 0), Qt::DisplayRole, text, 1, Qt::MatchContains | Qt::MatchRecursive);
    if (result.isEmpty())
        return;
    const auto index = result.first();
    ui->dependencyView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    ui->dependencyView->scrollTo(index);
}

void DependencyView::dependencySelected(const QItemSelection& selection)
{
    if (selection.isEmpty())
        return;
    const auto idx = selection.first().topLeft();

    auto user = idx.data(DependencyModel::UserFileRole).value<ElfFile*>();
    auto provider = idx.data(DependencyModel::ProviderFileRole).value<ElfFile*>();
    m_symbolModel->setFiles(user, provider);
}

void DependencyView::inverseFileSelected(const QItemSelection& selection)
{
    if (selection.isEmpty()) {
        m_fileUserModel->setFile(nullptr, nullptr);
        return;
    }

    const auto idx = selection.first().topLeft();
    m_fileUserModel->setFile(m_fileListModel->fileSet(), idx.data(FileListModel::FileRole).value<ElfFile*>());
}

void DependencyView::inverseUserSelected(const QItemSelection& selection)
{
    if (selection.isEmpty()) {
        m_inverseSymbolModel->setFiles(nullptr, nullptr);
        return;
    }

    const auto idx = selection.first().topLeft();
    m_inverseSymbolModel->setFiles(idx.data(FileUserModel::FileRole).value<ElfFile*>(), m_fileUserModel->usedFile());
}
