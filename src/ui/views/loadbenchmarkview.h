/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef LOADBENCHMARKVIEW_H
#define LOADBENCHMARKVIEW_H

#include <QWidget>

#include <memory>

namespace Ui {
class LoadBenchmarkView;
}
class LoadBenchmarkModel;
class LDBenchmark;

class ElfFileSet;

class LoadBenchmarkView : public QWidget
{
    Q_OBJECT
public:
    explicit LoadBenchmarkView(QWidget* parent = nullptr);
    ~LoadBenchmarkView();

    void setFileSet(ElfFileSet *fileSet);

private Q_SLOTS:
    void runBenchmark();

private:
    std::unique_ptr<Ui::LoadBenchmarkView> ui;
    ElfFileSet *m_fileSet = nullptr;
    LoadBenchmarkModel *m_model;
    std::shared_ptr<LDBenchmark> m_benchmark;
};

#endif // LOADBENCHMARKVIEW_H
