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

#ifndef ELFSTRUCTUREVIEW_H
#define ELFSTRUCTUREVIEW_H

#include <QVector>
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

private slots:
    void selectionChanged(const QItemSelection& selection);
    void anchorClicked(const QUrl &url);

private:
    void updateActionState();
    void selectUrl(const QUrl &url);

    std::unique_ptr<Ui::ElfStructureView> ui;
    ElfModel *m_elfModel = nullptr;
    QSortFilterProxyModel* m_proxy;
    QVector<QUrl> m_history;
    int m_historyIndex = -1;
    bool m_historyLock = false;
};

#endif // ELFSTRUCTUREVIEW_H
