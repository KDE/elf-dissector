/*
    Copyright (C) 2014-2015 Volker Krause <vkrause@kde.org>

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

#ifndef DWARFINFO_H
#define DWARFINFO_H

#include <elf/elffile.h>

#include <libdwarf.h>

#include <memory>

class DwarfCuDie;
class DwarfDie;
class DwarfInfoPrivate;
class DwarfAddressRanges;

/** Represents the .debug_info section. */
class DwarfInfo
{
public:
    explicit DwarfInfo(ElfFile* elfFile);
    DwarfInfo(const DwarfInfo&) = delete;
    ~DwarfInfo();

    DwarfInfo& operator=(const DwarfInfo&) = delete;

    /** The ELF file this DWARF information belong to. */
    ElfFile* elfFile() const;

    /** The corresponding .debug_arange section. Use for address-based lookups. */
    DwarfAddressRanges* addressRanges() const;

    DwarfDie* dieForMangledSymbol(const QByteArray &symbol) const;

    Dwarf_Debug dwarfHandle() const; // TODO this shouldn't be public API

    QVector<DwarfCuDie*> compilationUnits() const;
    /** Returns the CU DIE for the given address.
     *  Prefer this over direct .debug_arange lookup, as that is not always
     *  available.
     */
    DwarfCuDie* compilationUnitForAddress(uint64_t address) const;

    DwarfDie* dieAtOffset(Dwarf_Off offset) const;

    bool isValid() const;
private:
    std::unique_ptr<DwarfInfoPrivate> d;
};

#endif // DWARFINFO_H
