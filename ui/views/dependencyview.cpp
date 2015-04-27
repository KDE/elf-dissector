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

#include <elf/elffileset.h>
#include <elf/elffile.h>

#include <QDebug>
#include <QStandardItemModel>

DependencyView::DependencyView(QWidget* parent):
    QWidget(parent),
    ui(new Ui::DependencyView),
    m_model(new QStandardItemModel(this))
{
    ui->setupUi(this);
    ui->dependencyView->setModel(m_model);

    addActions({
        ui->actionExpandAll,
        ui->actionCollapseAll
    });
}

DependencyView::~DependencyView() = default;

void DependencyView::setFileSet(ElfFileSet* fileSet)
{
    m_model->clear();
    m_fileIndex.clear();
    m_fileSet = fileSet;
    if (!fileSet || fileSet->size() == 0)
        return;

    for (int i = 0; i < m_fileSet->size(); ++i) {
        const auto file = m_fileSet->file(i);
        const auto soName = file->dynamicSection()->soName();
        if (!soName.isEmpty())
            m_fileIndex.insert(soName, file);
    }

    auto file = fileSet->file(0);
    auto root = new QStandardItem(file->displayName()); // TODO soName vs. fileName for non-SOs
//     root->setToolTip(file->displayName());
    m_model->appendRow(root);
    buildTree(root, fileSet->file(0));
}

void DependencyView::buildTree(QStandardItem* parent, ElfFile* file)
{
    for (const auto &needed : file->dynamicSection()->neededLibraries()) {
        const auto depFile = findFile(needed);
        auto item = new QStandardItem(QString::fromUtf8(needed));
        if (depFile) {
            item->setToolTip(depFile->displayName());
            if (!hasCycle(item, needed)) {
                buildTree(item, depFile);
            } else {
                item->setIcon(QIcon::fromTheme("dialog-warning")); // cycle, TODO: better warning and message
            }
        } else {
            // dependency not found
            item->setIcon(QIcon::fromTheme("dialog-error"));
        }
        parent->appendRow(item);
    }
}

ElfFile* DependencyView::findFile(const QByteArray& soName) const
{
    const auto it = m_fileIndex.constFind(soName);
    if (it == m_fileIndex.constEnd())
        return nullptr;
    return it.value();
}

bool DependencyView::hasCycle(QStandardItem* item, const QByteArray& soName) const
{
    while (QStandardItem *parent = item->parent()) {
        if (parent->text() == soName)
            return true;
    }
    return false;
}
