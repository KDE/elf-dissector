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

#ifndef SECTIONPROXYMODEL_H
#define SECTIONPROXYMODEL_H

#include <QSortFilterProxyModel>

/** Restrict the structure model to just files and sections. */
class SectionProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SectionProxyModel(QObject* parent = nullptr);
    ~SectionProxyModel();

    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const final override;
};

#endif // SECTIONPROXYMODEL_H
