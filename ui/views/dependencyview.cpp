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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "dependencyview.h"
#include "ui_dependencyview.h"
#include <dependencymodel/dependencymodel.h>
#include <dependencymodel/usedsymbolmodel.h>

#include <elf/elffile.h>

#include <QDebug>
#include <QSortFilterProxyModel>

DependencyView::DependencyView(QWidget* parent):
    QWidget(parent),
    ui(new Ui::DependencyView),
    m_dependencyModel(new DependencyModel(this)),
    m_symbolModel(new UsedSymbolModel(this))
{
    ui->setupUi(this);
    ui->dependencyView->setModel(m_dependencyModel);
    ui->dependencyView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    auto proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(m_symbolModel);
    ui->symbolView->setModel(proxy);

    connect(ui->dependencySearchLine, &QLineEdit::textChanged, this, &DependencyView::search);
    connect(ui->dependencyView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DependencyView::dependencySelected);
    connect(ui->symbolSearchLine, &QLineEdit::textChanged, this, [proxy](const QString &text) {
        proxy->setFilterFixedString(text);
    });
}

DependencyView::~DependencyView() = default;

void DependencyView::setFileSet(ElfFileSet* fileSet)
{
    m_dependencyModel->setFileSet(fileSet);
    if (m_dependencyModel->rowCount() > 0)
        ui->dependencyView->expand(m_dependencyModel->index(0, 0));
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
