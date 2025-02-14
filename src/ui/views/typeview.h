/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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

private Q_SLOTS:
    void selectionChanged(const QItemSelection& selection);

private:
    std::unique_ptr<Ui::TypeView> ui;
    ElfFileSet *m_fileSet = nullptr;
    TypeModel *m_model;
};

#endif // TYPEVIEW_H
