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

#include "sidepane.h"

#include <QStyledItemDelegate>

class Delegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit Delegate(QObject *parent = nullptr) :
        QStyledItemDelegate(parent)
    {
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        auto size = QStyledItemDelegate::sizeHint(option, index);
        if (!option.widget || !index.isValid())
            return size;
        size.setWidth(std::max(size.width(), option.widget->minimumWidth()));
        return size;
    }
};

class NonEmptySelectionModel : public QItemSelectionModel
{
    Q_OBJECT
public:
    explicit NonEmptySelectionModel(QAbstractItemModel* model) :
        QItemSelectionModel(model)
    {
    }

    void select(const QItemSelection& selection, SelectionFlags command) override
    {
        if (selection.isEmpty() && (command & QItemSelectionModel::Clear))
            return;
        QItemSelectionModel::select(selection, command);
    }

    void select(const QModelIndex& index, SelectionFlags command) override
    {
        if (!index.isValid() && (command & QItemSelectionModel::Clear))
            return;
        QItemSelectionModel::select(index, command);
    }
};


SidePane::SidePane(QWidget* parent):
    QListView(parent)
{
    viewport()->setAutoFillBackground(false);
    setItemDelegate(new Delegate(this));
}

SidePane::~SidePane() = default;

QSize SidePane::sizeHint() const
{
    if (!model()) {
        return QSize(0, 0);
    }

    const int width = sizeHintForColumn(0);
    const int height = QListView::sizeHint().height();

    return QSize(width, height);
}

void SidePane::setModel(QAbstractItemModel* model)
{
    QAbstractItemView::setModel(model);
    setMinimumWidth(sizeHint().width());
    if (model) {
        auto selModel = new NonEmptySelectionModel(model);
        setSelectionModel(selModel);
        connect(selModel, &QItemSelectionModel::selectionChanged, this, [this]() {
            const auto selection = selectionModel()->selectedRows();
            if (selection.isEmpty())
                return;
            emit currentIndexChanged(selection.first().row());
        });
    }
}

#include "sidepane.moc"
