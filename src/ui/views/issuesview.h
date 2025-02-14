/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ISSUESVIEW_H
#define ISSUESVIEW_H

#include <QWidget>

#include <memory>

class ElfFileSet;
class IssuesModel;
namespace Ui
{
class IssuesView;
}

class IssuesView : public QWidget
{
    Q_OBJECT
public:
    explicit IssuesView(QWidget* parent = nullptr);
    ~IssuesView();

    void setFileSet(ElfFileSet *fileSet);

private:
    std::unique_ptr<Ui::IssuesView> ui;
    IssuesModel *m_issuesModel;
};

#endif // ISSUESVIEW_H
