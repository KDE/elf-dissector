/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PRINTERUTILS_P_H
#define PRINTERUTILS_P_H

#include <QByteArray>
#include <QList>

template <typename T>
struct LookupTableEntry
{
    T value;
    const char* const label;
};

// NOTE: the tables don't have to be sorted, so these lookups are linear.

template <typename T, std::size_t N>
QByteArray lookupLabel(T value, const LookupTableEntry<T> (&lookupTable)[N])
{
    for (std::size_t i = 0; i < N; ++i) {
        if (lookupTable[i].value == value)
            return QByteArray::fromRawData(lookupTable[i].label, strlen(lookupTable[i].label));
    }
    return QByteArray("unknown (") + QByteArray::number(value) + ')';
}

template <typename T, std::size_t N>
QByteArray lookupFlags(T flags, const LookupTableEntry<T> (&lookupTable)[N])
{
    QList<QByteArray> l;
    T handledFlags = 0;

    for (std::size_t i = 0; i < N; ++i) {
        if (flags & lookupTable[i].value)
            l.push_back(QByteArray::fromRawData(lookupTable[i].label, strlen(lookupTable[i].label)));
        handledFlags |= lookupTable[i].value;
    }

    if (flags & ~handledFlags)
        l.push_back(QByteArray("unhandled flag 0x") + QByteArray::number(qulonglong(flags & ~handledFlags), 16));

    if (l.isEmpty())
        return "none";
    return l.join(", ");
}

#endif
