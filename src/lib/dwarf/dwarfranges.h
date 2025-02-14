/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DWARFRANGES_H
#define DWARFRANGES_H

#include <QMetaType>

#include <libdwarf.h>

#include <memory>

class DwarfDie;

/** Represents the value of an DW_AT_ranges attribute. */
class DwarfRanges
{
public:
    DwarfRanges();
    explicit DwarfRanges(const DwarfDie* die, uint64_t offset);
    DwarfRanges(const DwarfRanges &other);
    DwarfRanges(DwarfRanges &&other);
    ~DwarfRanges();

    DwarfRanges& operator=(const DwarfRanges &other);
    DwarfRanges& operator=(DwarfRanges &&other);

    bool isValid() const;
    int size() const;
    Dwarf_Ranges* entry(int index) const;

private:
    std::shared_ptr<Dwarf_Ranges> m_ranges;
    Dwarf_Signed m_rangeSize;
};

Q_DECLARE_METATYPE(DwarfRanges)

#endif // DWARFRANGES_H
