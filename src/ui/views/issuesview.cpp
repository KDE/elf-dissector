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

#include "issuesview.h"
#include "ui_issuesview.h"

#include <issuesmodel/issuesmodel.h>

#include <QSortFilterProxyModel>

IssuesView::IssuesView(QWidget* parent):
    QWidget(parent),
    ui(new Ui::IssuesView),
    m_issuesModel(new IssuesModel(this))
{
    ui->setupUi(this);

    auto proxy = new QSortFilterProxyModel(this);
    proxy->setFilterKeyColumn(-1);
    proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxy->setSourceModel(m_issuesModel);
    ui->issuesView->setModel(proxy);
    ui->issuesView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    connect(ui->searchLine, &QLineEdit::textChanged, this, [proxy](const QString &text) {
        proxy->setFilterFixedString(text);
    });

    connect(ui->checkButton, &QPushButton::clicked, m_issuesModel, &IssuesModel::runChecks);
}

IssuesView::~IssuesView() = default;

void IssuesView::setFileSet(ElfFileSet* fileSet)
{
    m_issuesModel->setFileSet(fileSet);
}
