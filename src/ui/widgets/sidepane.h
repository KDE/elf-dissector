/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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

Q_SIGNALS:
    void currentIndexChanged(int index);
};

#endif // SIDEPANE_H
