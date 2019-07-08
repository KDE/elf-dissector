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

#ifndef TYPEVIEW_H
#define TYPEVIEW_H

#include <QWidget>

#include <memory>

namespace Ui
{
class TypeView;
}

class ElfFileSet;
class TypeModel;
class QItemSelection;

class TypeView : public QWidget
{
    Q_OBJECT
public:
    explicit TypeView(QWidget* parent = nullptr);
    ~TypeView();

    void setFileSet(ElfFileSet *fileSet);

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void selectionChanged(const QItemSelection& selection);

private:
    std::unique_ptr<Ui::TypeView> ui;
    ElfFileSet *m_fileSet = nullptr;
    TypeModel *m_model;
};

#endif // TYPEVIEW_H
