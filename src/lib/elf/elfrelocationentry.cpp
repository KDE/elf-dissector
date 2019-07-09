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

#include "elfrelocationentry.h"
#include "elfrelocationsection.h"
#include "elffile.h"
#include "elfsymboltablesection.h"

#include <elf.h>

ElfRelocationEntry::ElfRelocationEntry() :
    m_section(nullptr),
    m_index(0),
    m_withAddend(0)
{
}

ElfRelocationEntry::ElfRelocationEntry(const ElfRelocationEntry&) = default;

ElfRelocationEntry::ElfRelocationEntry(const ElfRelocationSection* section, uint64_t index, bool withAddend) :
    m_section(section),
    m_index(index),
    m_withAddend(withAddend)
{
}

ElfRelocationEntry::~ElfRelocationEntry() = default;

ElfRelocationEntry& ElfRelocationEntry::operator=(const ElfRelocationEntry&) = default;

const ElfRelocationSection* ElfRelocationEntry::relocationTable() const
{
    return m_section;
}

uint64_t ElfRelocationEntry::offset() const
{
    if (is64()) {
        if (m_withAddend)
            return entry<Elf64_Rela>()->r_offset;
        else
            return entry<Elf64_Rel>()->r_offset;
    } else {
        if (m_withAddend)
            return entry<Elf32_Rela>()->r_offset;
        else
            return entry<Elf32_Rel>()->r_offset;
    }
    Q_UNREACHABLE();
}

uint32_t ElfRelocationEntry::symbolIndex() const
{
    if (is64()) {
        if (m_withAddend)
            return ELF64_R_SYM(entry<Elf64_Rela>()->r_info);
        else
            return ELF64_R_SYM(entry<Elf64_Rel>()->r_info);
    } else {
        if (m_withAddend)
            return ELF32_R_SYM(entry<Elf32_Rela>()->r_info);
        else
            return ELF32_R_SYM(entry<Elf32_Rel>()->r_info);
    }
    Q_UNREACHABLE();
}

uint32_t ElfRelocationEntry::type() const
{
    if (is64()) {
        if (m_withAddend)
            return ELF64_R_TYPE(entry<Elf64_Rela>()->r_info);
        else
            return ELF64_R_TYPE(entry<Elf64_Rel>()->r_info);
    } else {
        if (m_withAddend)
            return ELF32_R_TYPE(entry<Elf32_Rela>()->r_info);
        else
            return ELF32_R_TYPE(entry<Elf32_Rel>()->r_info);
    }
    Q_UNREACHABLE();
}

uint64_t ElfRelocationEntry::addend() const
{
    if (m_withAddend) {
        if (is64()) {
            return entry<Elf64_Rela>()->r_addend;
        } else {
            return entry<Elf32_Rela>()->r_addend;
        }
    } else {
        // TODO
        Q_ASSERT_X(false, "", "not yet implemented!");
        return 0;
    }
    Q_UNREACHABLE();
}

template <typename T>
const T* ElfRelocationEntry::entry() const
{
    return reinterpret_cast<const T*>(m_section->rawData() + m_index * m_section->header()->entrySize());
}

bool ElfRelocationEntry::is64() const
{
    return m_section->file()->type() == ELFCLASS64;
}

ElfSymbolTableEntry* ElfRelocationEntry::symbol() const
{
    const auto symIdx = symbolIndex();
    if (symIdx > 0)
        return m_section->linkedSection<ElfSymbolTableSection>()->entry(symIdx);
    return nullptr;
}

uint64_t ElfRelocationEntry::relocationTarget() const
{
    // ### TODO ### this is utterly incomplete and largely incorrect!!
    // this would need to implement the platform-specific relocation logic for all types
    // the below approximation just barely covers the current uses of this function

    if (symbolIndex() && addend()) {
        Q_ASSERT_X(false, "", "not yet implemented!");
        return 0;
    }

    if (symbolIndex()) {
        return symbol()->value();
    }
    return addend();
}
