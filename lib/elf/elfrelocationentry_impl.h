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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ELFRELOCATIONENTRY_IMPL_H
#define ELFRELOCATIONENTRY_IMPL_H

#include "elfrelocationentry.h"
#include "elfrelocationsection.h"

#include <elf.h>

template <typename T>
class ElfRelocationEntryImpl : public ElfRelocationEntry
{
public:
    explicit ElfRelocationEntryImpl(const ElfRelocationSection *section, uint32_t index) :
        ElfRelocationEntry(section),
        m_entry(reinterpret_cast<const T*>(section->rawData() + index * section->header()->entrySize()))
    {
    }

    uint64_t offset() const final
    {
        return m_entry->r_offset;
    }

    uint32_t symbol() const final
    {
        if (relocationTable()->file()->type() == ELFCLASS64)
            return ELF64_R_SYM(m_entry->r_info);
        if (relocationTable()->file()->type() == ELFCLASS32)
            return ELF32_R_SYM(m_entry->r_info);
        Q_UNREACHABLE();
    }

    uint32_t type() const final
    {
        if (relocationTable()->file()->type() == ELFCLASS64)
            return ELF64_R_TYPE(m_entry->r_info);
        if (relocationTable()->file()->type() == ELFCLASS32)
            return ELF32_R_TYPE(m_entry->r_info);
        Q_UNREACHABLE();
    }

    uint64_t addend() const override
    {
        // TODO
        return 0;
    }

protected:
    const T* const m_entry;
};

template <typename T>
class ElfRelocationEntryImplA : public ElfRelocationEntryImpl<T>
{
public:
    explicit ElfRelocationEntryImplA(const ElfRelocationSection *section, uint32_t index) :
        ElfRelocationEntryImpl<T>(section, index)
    {}

    uint64_t addend() const final
    {
        return ElfRelocationEntryImpl<T>::m_entry->r_addend;
    }
};

#endif
