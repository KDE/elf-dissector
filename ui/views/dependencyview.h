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

#ifndef DEPENDENCYVIEW_H
#define DEPENDENCYVIEW_H

#include <QWidget>

#include <memory>

namespace Ui {
class DependencyView;
}

class ElfFileSet;
class DependencyModel;
class UsedSymbolModel;
class UnusedDependenciesModel;
class FileListModel;
class FileUserModel;

class QItemSelection;

class DependencyView : public QWidget
{
    Q_OBJECT
public:
    explicit DependencyView(QWidget* parent = nullptr);
    ~DependencyView();

    void setFileSet(ElfFileSet *fileSet);

private slots:
    void search(const QString &text);
    void dependencySelected(const QItemSelection &selection);
    void inverseFileSelected(const QItemSelection &selection);
    void inverseUserSelected(const QItemSelection &selection);

private:
    std::unique_ptr<Ui::DependencyView> ui;
    DependencyModel *m_dependencyModel;
    UsedSymbolModel *m_symbolModel;
    UnusedDependenciesModel *m_unusedDependenciesModel;
    FileListModel *m_fileListModel;
    FileUserModel *m_fileUserModel;
    UsedSymbolModel *m_inverseSymbolModel;
};

#endif // DEPENDENCYVIEW_H
