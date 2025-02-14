/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
