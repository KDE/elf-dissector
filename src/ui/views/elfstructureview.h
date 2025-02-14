/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFSTRUCTUREVIEW_H
#define ELFSTRUCTUREVIEW_H

#include <QList>
#include <QWidget>

#include <memory>

namespace Ui {
class ElfStructureView;
}

class ElfModel;
class QItemSelection;
class QSortFilterProxyModel;
class QUrl;

class ElfStructureView : public QWidget
{
    Q_OBJECT
public:
    explicit ElfStructureView(QWidget* parent = nullptr);
    ~ElfStructureView();

    void setModel(ElfModel* model);

protected:
    bool eventFilter(QObject* receiver, QEvent* event) override;

private Q_SLOTS:
    void selectionChanged(const QItemSelection& selection);
    void anchorClicked(const QUrl &url);

private:
    void updateActionState();
    void selectUrl(const QUrl &url);

    std::unique_ptr<Ui::ElfStructureView> ui;
    ElfModel *m_elfModel = nullptr;
    QSortFilterProxyModel* m_proxy;
    QList<QUrl> m_history;
    int m_historyIndex = -1;
    bool m_historyLock = false;
};

#endif // ELFSTRUCTUREVIEW_H
