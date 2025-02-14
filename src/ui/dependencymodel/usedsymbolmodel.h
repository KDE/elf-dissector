/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef USEDSYMBOLMODEL_H
#define USEDSYMBOLMODEL_H

#include <QAbstractListModel>
#include <QList>

class ElfFile;
class ElfSymbolTableEntry;

class UsedSymbolModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit UsedSymbolModel(QObject* parent = nullptr);
    ~UsedSymbolModel();

    void setFiles(ElfFile *user, ElfFile *provider);

    QVariant data(const QModelIndex& index, int role) const override;
    int rowCount(const QModelIndex& parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    QList<ElfSymbolTableEntry*> m_entries;
};

#endif // USEDSYMBOLMODEL_H
