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

#include "elfsymboltableentry.h"
#include "elfsymboltablesection.h"
#include "elfstringtablesection.h"
#include "elffile.h"
#include "elfheader.h"


#include <elf.h>

ElfSymbolTableEntry::ElfSymbolTableEntry(): m_section(nullptr)
{
    m_symbol.sym32 = nullptr;
}

ElfSymbolTableEntry::ElfSymbolTableEntry(const ElfSymbolTableSection* section, uint32_t index) :
    m_section(section)
{
    // sym64 will contain the right pointer as well
    m_symbol.sym32 = reinterpret_cast<Elf32_Sym*>(section->rawData() + index * section->header()->entrySize());
}

uint32_t ElfSymbolTableEntry::nameIndex() const
{
    if (m_section->file()->type() == ELFCLASS64)
        return m_symbol.sym64->st_name;
    return m_symbol.sym32->st_name;
}

uint8_t ElfSymbolTableEntry::info() const
{
    if (m_section->file()->type() == ELFCLASS64)
        return m_symbol.sym64->st_info;
    return m_symbol.sym32->st_info;
}

uint8_t ElfSymbolTableEntry::other() const
{
    if (m_section->file()->type() == ELFCLASS64)
        return m_symbol.sym64->st_other;
    return m_symbol.sym32->st_other;
}

uint16_t ElfSymbolTableEntry::sectionIndex() const
{
    if (m_section->file()->type() == ELFCLASS64)
        return m_symbol.sym64->st_shndx;
    return m_symbol.sym32->st_shndx;
}

uint64_t ElfSymbolTableEntry::value() const
{
    if (m_section->file()->type() == ELFCLASS64)
        return m_symbol.sym64->st_value;
    return m_symbol.sym32->st_value;
}

uint64_t ElfSymbolTableEntry::size() const
{
    if (m_section->file()->type() == ELFCLASS64)
        return m_symbol.sym64->st_size;
    return m_symbol.sym32->st_size;
}

uint32_t ElfSymbolTableEntry::index() const
{
    return (reinterpret_cast<const unsigned char*>(m_symbol.sym32) - symbolTable()->rawData()) / symbolTable()->header()->entrySize();
}

const ElfSymbolTableSection* ElfSymbolTableEntry::symbolTable() const
{
    return m_section;
}

const char* ElfSymbolTableEntry::name() const
{
    return m_section->linkedSection<ElfStringTableSection>()->string(nameIndex());
}

bool ElfSymbolTableEntry::hasValidSection() const
{
    const auto index = sectionIndex();
    if (index >= symbolTable()->file()->header()->sectionHeaderCount())
        return false;
    return symbolTable()->file()->sectionHeaders().at(index)->type() != SHT_NULL;
}

ElfSectionHeader* ElfSymbolTableEntry::sectionHeader() const
{
    return symbolTable()->file()->sectionHeaders().at(sectionIndex());
}

ElfSection* ElfSymbolTableEntry::section() const
{
    return symbolTable()->file()->section<ElfSection>(sectionIndex());
}

uint8_t ElfSymbolTableEntry::bindType() const
{
    // same as 64
    return ELF32_ST_BIND(info());
}

uint8_t ElfSymbolTableEntry::type() const
{
    // same as 64
    return ELF32_ST_TYPE(info());
}

uint8_t ElfSymbolTableEntry::visibility() const
{
    // same as 64
    return ELF32_ST_VISIBILITY(other());
}

const unsigned char* ElfSymbolTableEntry::data() const
{
    const auto targetSection = m_section->file()->section<ElfSection>(sectionIndex());
    return targetSection->rawData() + value() - targetSection->header()->virtualAddress();
}
