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

private slots:
    void runBenchmark();

private:
    std::unique_ptr<Ui::LoadBenchmarkView> ui;
    ElfFileSet *m_fileSet = nullptr;
    LoadBenchmarkModel *m_model;
    std::shared_ptr<LDBenchmark> m_benchmark;
};

#endif // LOADBENCHMARKVIEW_H
