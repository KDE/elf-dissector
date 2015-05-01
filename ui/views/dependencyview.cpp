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

#include "dependencyview.h"
#include "ui_dependencyview.h"
#include "dependencymodel/dependencymodel.h"

#include <QDebug>

DependencyView::DependencyView(QWidget* parent):
    QWidget(parent),
    ui(new Ui::DependencyView),
    m_dependencyModel(new DependencyModel(this))
{
    ui->setupUi(this);
    ui->dependencyView->setModel(m_dependencyModel);

    addActions({
        ui->actionExpandAll,
        ui->actionCollapseAll
    });
}

DependencyView::~DependencyView() = default;

void DependencyView::setFileSet(ElfFileSet* fileSet)
{
    m_dependencyModel->setFileSet(fileSet);
}
