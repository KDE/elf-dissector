/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ISSUESMODEL_H
#define ISSUESMODEL_H

#include <checks/virtualdtorcheck.h>

#include <QAbstractTableModel>

class ElfFileSet;
class IssuesModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit IssuesModel(QObject* parent = nullptr);
    ~IssuesModel();

    void setFileSet(ElfFileSet *fileSet);
    void runChecks();

    QVariant data(const QModelIndex& index, int role) const override;
    int columnCount(const QModelIndex& parent) const override;
    int rowCount(const QModelIndex& parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    ElfFileSet *m_fileSet;
    VirtualDtorCheck m_checker;
};

#endif // ISSUESMODEL_H
