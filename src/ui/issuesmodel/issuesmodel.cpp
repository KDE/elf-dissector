/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "issuesmodel.h"

#include <memory>

IssuesModel::IssuesModel(QObject* parent): QAbstractTableModel(parent)
{
}

IssuesModel::~IssuesModel() = default;

void IssuesModel::setFileSet(ElfFileSet* fileSet)
{
    beginResetModel();
    const auto l = [](decltype(this) m) { m->endResetModel(); };
    const auto endReset = std::unique_ptr<IssuesModel, decltype(l)>(this, l);

    m_fileSet = fileSet;
    m_checker.clear();
}

void IssuesModel::runChecks()
{
    if (!m_checker.results().isEmpty())
        return;

    beginResetModel();
    const auto l = [](decltype(this) m) { m->endResetModel(); };
    const auto endReset = std::unique_ptr<IssuesModel, decltype(l)>(this, l);

    if (m_fileSet)
        m_checker.findImplicitVirtualDtors(m_fileSet);
}

QVariant IssuesModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return {};

    const auto res = m_checker.results().at(index.row());
    switch (role) {
        case Qt::DisplayRole:
        {
            switch (index.column()) {
                case 0: return tr("%1 (implicit virtual destructor)").arg(QString::fromLatin1(res.fullName));
                case 1: return QString(res.sourceFilePath + QLatin1Char(':') + QString::number(res.lineNumber));
            }
        }
    }

    return {};
}

int IssuesModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 2;
}

int IssuesModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return m_checker.results().size();
}

QVariant IssuesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0: return tr("Issue");
            case 1: return tr("Location");
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}
