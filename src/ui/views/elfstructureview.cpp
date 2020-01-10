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

#include "elfstructureview.h"
#include "ui_elfstructureview.h"

#include <elfmodel/elfmodel.h>
#include <navigator/codenavigator.h>

#include <QSortFilterProxyModel>

#include <QMouseEvent>

ElfStructureView::ElfStructureView(QWidget* parent):
    QWidget(parent),
    ui(new Ui::ElfStructureView),
    m_proxy(new QSortFilterProxyModel(this))
{
    ui->setupUi(this);
    ui->elfStructureView->setModel(m_proxy);
    m_proxy->setRecursiveFilteringEnabled(true);
    ui->elfStructureView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(ui->elfStructureView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &ElfStructureView::selectionChanged);
    connect(ui->elfStructureSearchLine, &QLineEdit::textChanged, this, [this](const QString &text) {
        m_proxy->setFilterFixedString(text);
    });
    connect(ui->elfDetailView, &QTextBrowser::anchorClicked, this, &ElfStructureView::anchorClicked);

    ui->actionBack->setShortcut(QKeySequence::Back);
    ui->actionForward->setShortcut(QKeySequence::Forward);

    connect(ui->actionBack, &QAction::triggered, this, [this]() {
        --m_historyIndex;
        m_historyLock = true;
        selectUrl(m_history.at(m_historyIndex));
        m_historyLock = false;
        updateActionState();
    });
    connect(ui->actionForward, &QAction::triggered, this, [this]() {
        ++m_historyIndex;
        m_historyLock = true;
        selectUrl(m_history.at(m_historyIndex));
        m_historyLock = false;
        updateActionState();
    });
    addActions({ ui->actionBack, ui->actionForward });
    updateActionState();

    installEventFilter(this);
}

ElfStructureView::~ElfStructureView() = default;

void ElfStructureView::setModel(ElfModel* model)
{
    m_elfModel = model;
    m_proxy->setSourceModel(model);
}

void ElfStructureView::selectionChanged(const QItemSelection &selection)
{
    if (selection.isEmpty()) {
        ui->elfDetailView->clear();
	return;
    }

    const QModelIndex index = selection.first().topLeft();
    ui->elfDetailView->setHtml(index.data(ElfModel::DetailRole).toString());

    if (m_historyLock)
        return;
    const auto url = index.data(ElfModel::NodeUrl).toUrl();
    if (url.isValid() && !url.isEmpty()) {
        m_history.resize(m_historyIndex + 2);
        m_history[++m_historyIndex] = url;
        updateActionState();
    }
}

void ElfStructureView::anchorClicked(const QUrl& url)
{
    if (url.scheme() == QLatin1String("code"))
        CodeNavigator::goTo(url);
    else if (url.scheme() == QLatin1String("elfmodel"))
        selectUrl(url);
}

void ElfStructureView::updateActionState()
{
    ui->actionBack->setEnabled(m_historyIndex > 0);
    ui->actionForward->setEnabled(m_historyIndex + 1 < m_history.size());
}

void ElfStructureView::selectUrl(const QUrl& url)
{
    auto idx = m_elfModel->indexForUrl(url);
    idx = m_proxy->mapFromSource(idx);
    ui->elfStructureView->selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    ui->elfStructureView->selectionModel()->setCurrentIndex(idx, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    ui->elfStructureView->scrollTo(idx);
}

bool ElfStructureView::eventFilter(QObject *receiver, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease) {
        auto mouseEv = static_cast<QMouseEvent*>(event);
        if (mouseEv->button() == Qt::BackButton) {
            if (ui->actionBack->isEnabled())
                ui->actionBack->trigger();
            return true;
        } else if (mouseEv->button() == Qt::ForwardButton) {
            if (ui->actionForward->isEnabled())
                ui->actionForward->trigger();
            return true;
        }
    }
    return QObject::eventFilter(receiver, event);
}
