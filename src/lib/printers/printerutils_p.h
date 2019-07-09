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

// TODO optimize for sorted tables
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
