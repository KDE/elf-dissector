/*
    Copyright (C) 2013-2014 Volker Krause <vkrause@kde.org>

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

#ifndef ELFSYMBOLTABLEENTRY_H
#define ELFSYMBOLTABLEENTRY_H

#include <cstdint>
#include <elf.h>

class ElfSectionHeader;
class ElfSection;
class ElfSymbolTableSection;

/** Symbol table entry. */
class ElfSymbolTableEntry
{
public:
    ElfSymbolTableEntry();
    explicit ElfSymbolTableEntry(const ElfSymbolTableSection* section, uint32_t index);

    const ElfSymbolTableSection* symbolTable() const;

    uint16_t sectionIndex() const;
    uint64_t value() const;
    uint64_t size() const;

    /** Bind type. */
    uint8_t bindType() const;
    /** Symbol type. */
    uint8_t type() const;
    /** Symbol visibility. */
    uint8_t visibility() const;

    /** Pointer to the associated code/data. */
    const unsigned char* data() const;

    /** Mangled name from string table. */
    const char* name() const;

    /** Returns true if this symbol is in a valid section. */
    bool hasValidSection() const;
    /** Header of the section this symbol is in. */
    ElfSectionHeader* sectionHeader() const;
    /** Section this symbol is in. */
    ElfSection* section() const;

    /** Index in the symbol table. */
    uint32_t index() const;

private:
    uint32_t nameIndex() const;
    uint8_t info() const;
    uint8_t other() const;

    const ElfSymbolTableSection *m_section;
    union {
        Elf32_Sym* sym32;
        Elf64_Sym* sym64;
    } m_symbol;
};

#endif // ELFSYMBOLTABLEENTRY_H
