/*
    SPDX-FileCopyrightText: 2014-2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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

    QList<DwarfCuDie*> compilationUnits() const;
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
