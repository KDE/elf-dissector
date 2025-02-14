/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef UNUSEDDEPENDENCIESMODEL_H
#define UNUSEDDEPENDENCIESMODEL_H

#include <checks/dependenciescheck.h>

#include <QAbstractTableModel>

class UnusedDependenciesModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit UnusedDependenciesModel(QObject* parent = nullptr);
    ~UnusedDependenciesModel();

    void setFileSet(ElfFileSet *fileSet);
    void findUnusedDependencies();

    QVariant data(const QModelIndex& index, int role) const override;
    int columnCount(const QModelIndex& parent) const override;
    int rowCount(const QModelIndex& parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    ElfFileSet *m_fileSet = nullptr;
    DependenciesCheck::UnusedDependencies m_unusedDeps;
};

#endif // UNUSEDDEPENDENCIESMODEL_H
