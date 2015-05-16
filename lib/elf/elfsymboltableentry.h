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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ELFSYMBOLTABLEENTRY_H
#define ELFSYMBOLTABLEENTRY_H

#include <cstdint>

class ElfSymbolTableSection;

/** Symbol table entry. */
class ElfSymbolTableEntry
{
public:
    ElfSymbolTableEntry(const ElfSymbolTableEntry &other) = delete;
    virtual ~ElfSymbolTableEntry();
    ElfSymbolTableEntry& operator=(const ElfSymbolTableEntry &other) = delete;

    const ElfSymbolTableSection* symbolTable() const;

    virtual uint32_t nameIndex() const = 0;
    virtual uint16_t sectionIndex() const = 0;
    virtual uint64_t value() const = 0;
    virtual uint64_t size() const = 0;

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

    /** Index in the symbol table. */
    virtual uint32_t index() const = 0;

protected:
    explicit ElfSymbolTableEntry(const ElfSymbolTableSection *section);
    virtual uint8_t info() const = 0;
    virtual uint8_t other() const = 0;
    const ElfSymbolTableSection *m_section;

};

#endif // ELFSYMBOLTABLEENTRY_H
