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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DEPENDENCYVIEW_H
#define DEPENDENCYVIEW_H

#include <QWidget>

#include <memory>

namespace Ui {
class DependencyView;
}

class ElfFile;
class ElfFileSet;

class QStandardItem;
class QStandardItemModel;

class DependencyView : public QWidget
{
    Q_OBJECT
public:
    explicit DependencyView(QWidget* parent = 0);
    ~DependencyView();

    void setFileSet(ElfFileSet *fileSet);

private:
    void buildTree(QStandardItem *parent, ElfFile *file);
    ElfFile* findFile(const QByteArray &soName) const;
    bool hasCycle(QStandardItem* item, const QByteArray &soName) const;

    std::unique_ptr<Ui::DependencyView> ui;
    ElfFileSet *m_fileSet;
    QStandardItemModel *m_model;
    QHash<QByteArray, ElfFile*> m_fileIndex;
};

#endif // DEPENDENCYVIEW_H
