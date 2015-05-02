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

#include "elfstructureview.h"
#include "ui_elfstructureview.h"

#include <model/elfmodel.h>
#include <navigator/codenavigator.h>
#include <3rdparty/kitemmodels/krecursivefilterproxymodel.h>

ElfStructureView::ElfStructureView(QWidget* parent):
    QWidget(parent),
    ui(new Ui::ElfStructureView),
    m_proxy(new KRecursiveFilterProxyModel(this))
{
    ui->setupUi(this);
    ui->elfStructureView->setModel(m_proxy);
    ui->elfStructureView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(ui->elfStructureView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &ElfStructureView::selectionChanged);
    connect(ui->elfStructureSearchLine, &QLineEdit::textChanged, this, [this](const QString &text) {
        m_proxy->setFilterFixedString(text);
    });
    connect(ui->elfDetailView, &QTextBrowser::anchorClicked, this, [](const QUrl &url) {
        if (url.scheme() == QLatin1String("code"))
            CodeNavigator::goTo(url);
    });
}

ElfStructureView::~ElfStructureView() = default;

void ElfStructureView::setModel(QAbstractItemModel* model)
{
    m_proxy->setSourceModel(model);
}

void ElfStructureView::selectionChanged(const QItemSelection &selection)
{
    if (selection.isEmpty())
        ui->elfDetailView->clear();

    const QModelIndex index = selection.first().topLeft();
    ui->elfDetailView->setHtml(index.data(ElfModel::DetailRole).toString());
}
