/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SIZETREEMAPVIEW_H
#define SIZETREEMAPVIEW_H

#include <QWidget>

#include <memory>

namespace Ui {
class SizeTreeMapView;
}

class TreeMapWidget;

class QAbstractItemModel;
class QSortFilterProxyModel;

class ElfSectionHeader;

class SizeTreeMapView : public QWidget
{
    Q_OBJECT
public:
    explicit SizeTreeMapView(QWidget *parent = nullptr);
    ~SizeTreeMapView();

    void setModel(QAbstractItemModel *model);
    void restoreSettings();

private:
    bool isSectionHidden(ElfSectionHeader *shdr) const;

private Q_SLOTS:
    void reloadTreeMap();
    void treeMapContextMenu(const QPoint &pos);
    void viewActionToggled();

private:
    std::unique_ptr<Ui::SizeTreeMapView> ui;
    QSortFilterProxyModel *m_sectionProxy;
    TreeMapWidget *m_treeMap = nullptr;
    bool m_viewDirty = true;
};

#endif // SIZETREEMAPVIEW_H
