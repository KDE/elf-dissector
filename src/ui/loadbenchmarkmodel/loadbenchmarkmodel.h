/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
