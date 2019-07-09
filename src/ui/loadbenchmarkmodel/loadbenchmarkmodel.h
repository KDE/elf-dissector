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

#ifndef LOADBENCHMARKMODEL_H
#define LOADBENCHMARKMODEL_H

#include <QAbstractTableModel>

#include <memory>

class LDBenchmark;

/** Result table of the load benchmark results. */
class LoadBenchmarkModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit LoadBenchmarkModel(QObject* parent = nullptr);
    ~LoadBenchmarkModel();

    void setBenchmark(const std::shared_ptr<LDBenchmark> &data);

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    std::shared_ptr<LDBenchmark> m_data;
};

#endif // LOADBENCHMARKMODEL_H
