/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
