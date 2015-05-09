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

#include "typeview.h"
#include "ui_typeview.h"

#include <3rdparty/kitemmodels/krecursivefilterproxymodel.h>
#include <typemodel/typemodel.h>

#include <QItemSelectionModel>

TypeView::TypeView(QWidget* parent):
    QWidget(parent),
    ui(new Ui::TypeView),
    m_model(new TypeModel(this))
{
    ui->setupUi(this);

    auto proxy = new KRecursiveFilterProxyModel(this);
    proxy->setSourceModel(m_model);
    ui->typeTreeView->setModel(proxy);

    connect(ui->typeTreeView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &TypeView::selectionChanged);
    connect(ui->searchLineEdit, &QLineEdit::textChanged, this, [proxy](const QString &text) {
        proxy->setFilterFixedString(text);
    });
}

TypeView::~TypeView() = default;

void TypeView::setFileSet(ElfFileSet* fileSet)
{
    m_fileSet = fileSet;
    if (isVisible())
        m_model->setFileSet(m_fileSet);
}

void TypeView::showEvent(QShowEvent* event)
{
    if (isVisible() && m_model->rowCount() == 0 && m_fileSet)
        m_model->setFileSet(m_fileSet);
    QWidget::showEvent(event);
}

void TypeView::selectionChanged(const QItemSelection& selection)
{
    if (selection.isEmpty())
        ui->textBrowser->clear();

    const QModelIndex index = selection.first().topLeft();
    ui->textBrowser->setHtml(index.data(TypeModel::DetailRole).toString());
}
