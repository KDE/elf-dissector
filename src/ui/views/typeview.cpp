/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "typeview.h"
#include "ui_typeview.h"

#include <typemodel/typemodel.h>
#include <navigator/codenavigator.h>

#include <QItemSelectionModel>
#include <QMessageBox>
#include <QSortFilterProxyModel>

TypeView::TypeView(QWidget* parent):
    QWidget(parent),
    ui(new Ui::TypeView),
    m_model(new TypeModel(this))
{
    ui->setupUi(this);

    auto proxy = new QSortFilterProxyModel(this);
    proxy->setRecursiveFilteringEnabled(true);
    proxy->setSourceModel(m_model);
    ui->typeTreeView->setModel(proxy);
    ui->typeTreeView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    connect(ui->typeTreeView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &TypeView::selectionChanged);
    connect(ui->searchLineEdit, &QLineEdit::textChanged, this, [proxy](const QString &text) {
        proxy->setFilterFixedString(text);
    });
    connect(ui->detailsView, &QTextBrowser::anchorClicked, this, [](const QUrl &url) {
        if (url.scheme() == QLatin1String("code"))
            CodeNavigator::goTo(url);
    });

}

TypeView::~TypeView() = default;

void TypeView::setFileSet(ElfFileSet* fileSet)
{
    m_fileSet = fileSet;
    if (isVisible()) {
        m_model->setFileSet(m_fileSet);
        if (m_model->hasInvalidDies())
            QMessageBox::warning(this, tr("Invalid DWARF entries"),
                                 tr("An error occurred while reading DWARF data of some ELF objects, the tree will be incomplete."));
    }
}

void TypeView::showEvent(QShowEvent* event)
{
    if (isVisible() && m_model->rowCount() == 0 && m_fileSet) {
        const auto res = QMessageBox::question(this, tr("Compute Type Tree"), tr("Computing the type tree from DWARF data can take up to several minutes in which the application will not respond, and use up to 1.5GB of memory. Proceed anyway?"), QMessageBox::Yes, QMessageBox::Cancel);
        if (res == QMessageBox::Yes) {
            m_model->setFileSet(m_fileSet);
            if (m_model->hasInvalidDies())
                QMessageBox::warning(this, tr("Invalid DWARF entries"),
                                     tr("An error occurred while reading DWARF data of some ELF objects, the tree will be incomplete."));
        }
    }
    QWidget::showEvent(event);
}

void TypeView::selectionChanged(const QItemSelection& selection)
{
    if (selection.isEmpty())
        ui->detailsView->clear();

    const QModelIndex index = selection.first().topLeft();
    ui->detailsView->setHtml(index.data(TypeModel::DetailRole).toString());
}
