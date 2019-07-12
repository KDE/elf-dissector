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

#ifndef SIDEPANE_H
#define SIDEPANE_H

#include <QListView>

class SidePane : public QListView
{
    Q_OBJECT
public:
    explicit SidePane(QWidget* parent = nullptr);
    ~SidePane();

    QSize sizeHint() const override;
    void setModel(QAbstractItemModel* model) override;

signals:
    void currentIndexChanged(int index);
};

#endif // SIDEPANE_H
