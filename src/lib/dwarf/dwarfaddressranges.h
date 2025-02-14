/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DWARFADDRESSRANGES_H
#define DWARFADDRESSRANGES_H

#include <libdwarf.h>

#include <cstdint>

class DwarfInfo;
class DwarfCuDie;
class DwarfDie;

/** Representation of the .debug_aranges section. */
class DwarfAddressRanges
{
public:
    explicit DwarfAddressRanges(DwarfInfo *info);
    DwarfAddressRanges(const DwarfAddressRanges&) = delete;
    ~DwarfAddressRanges();

    DwarfAddressRanges& operator=(const DwarfAddressRanges&) = delete;

    /** Returns @c true if the .debug_aranges section is present. */
    bool isValid() const;

    /** Looks up the CU DIE for the given address. */
    DwarfCuDie* compilationUnitForAddress(uint64_t addr) const;
    /** Looks up the DIE for the given address. */
    DwarfDie* dieForAddress(uint64_t addr) const;

private:
    Dwarf_Arange *m_aranges;
    DwarfInfo *m_info;
    Dwarf_Signed m_arangesSize;
};

#endif // DWARFADDRESSRANGES_H
