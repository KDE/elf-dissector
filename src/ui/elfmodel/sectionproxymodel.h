/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
