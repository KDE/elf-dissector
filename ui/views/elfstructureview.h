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

#ifndef ELFSTRUCTUREVIEW_H
#define ELFSTRUCTUREVIEW_H

#include <QWidget>

#include <memory>

namespace Ui {
class ElfStructureView;
}

class QAbstractItemModel;
class QItemSelection;
class QSortFilterProxyModel;

class ElfStructureView : public QWidget
{
    Q_OBJECT
public:
    explicit ElfStructureView(QWidget* parent = 0);
    ~ElfStructureView();

    void setModel(QAbstractItemModel* model);

private slots:
    void selectionChanged(const QItemSelection& selection);

private:
    std::unique_ptr<Ui::ElfStructureView> ui;
    QSortFilterProxyModel* m_proxy;
};

#endif // ELFSTRUCTUREVIEW_H