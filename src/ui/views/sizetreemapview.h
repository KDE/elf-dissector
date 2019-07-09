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

private slots:
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
