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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DWARFDEBUG_H
#define DWARFDEBUG_H

#include <elf/elffile.h>

#include <libdwarf/libdwarf.h>

#include <memory>

class DwarfDie;
class DwarfInfoPrivate;

class DwarfInfo
{
public:
    explicit DwarfInfo(ElfFile* elfFile);
    DwarfInfo(const DwarfInfo&) = delete;
    ~DwarfInfo();

    DwarfInfo& operator=(const DwarfInfo&) = delete;

    /** The ELF file this DWARF information belong to. */
    ElfFile* elfFile() const;

    QString sourceLocationForMangledSymbol(const QByteArray &symbol) const;

    Dwarf_Debug dwarfHandle() const; // TODO this shouldn't be public API

    QVector<DwarfDie*> compilationUnits() const;

    DwarfDie* dieAtOffset(Dwarf_Off offset) const;

private:
    std::unique_ptr<DwarfInfoPrivate> d;
};

#endif // DWARFDEBUG_H
