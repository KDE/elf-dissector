/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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

private Q_SLOTS:
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
