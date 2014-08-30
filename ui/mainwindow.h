/*
    Copyright (C) 2013-2014 Volker Krause <vkrause@kde.org>

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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <memory>

class ElfFileSet;
class ElfModel;
class TreeMapWidget;

class QItemSelection;

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private slots:
    void fileOpen();
    void hideDebugInformation();
    void reloadFileOnStartup();
    void colorizationChanged();
    void treeMapContextMenu(const QPoint &pos);
    void selectionChanged(const QItemSelection &selection);

private:
    void restoreSettings();
    void loadFile(const QString &fileName);

private:
    std::unique_ptr<Ui::MainWindow> ui;
    TreeMapWidget *m_treeMap = 0;
    QString m_currentFileName;
    ElfModel *m_elfModel = 0;
    ElfFileSet *m_fileSet = 0;
};

#endif // MAINWINDOW_H
