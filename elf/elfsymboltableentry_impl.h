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

#ifndef ELFSYMBOLTABLEENTRY_IMPL_H
#define ELFSYMBOLTABLEENTRY_IMPL_H

#include "elfsymboltableentry.h"
#include "elfsymboltablesection.h"

template <typename T>
class ElfSymbolTableEntryImpl : public ElfSymbolTableEntry
{
public:
    explicit inline ElfSymbolTableEntryImpl(const ElfSymbolTableSection* section, uint32_t index) :
    ElfSymbolTableEntry(section),
    m_symbol(reinterpret_cast<const T*>(section->rawData() + index * section->header()->entrySize()))
    {
    }

    inline uint32_t nameIndex() const override
    {
        return m_symbol->st_name;
    }

    inline uint8_t info() const override
    {
        return m_symbol->st_info;
    }

    inline uint8_t other() const override
    {
        return m_symbol->st_other;
    }

    inline uint16_t sectionIndex() const override
    {
        return m_symbol->st_shndx;
    }

    inline uint64_t value() const override
    {
        return m_symbol->st_value;
    }

    inline uint64_t size() const override
    {
        return m_symbol->st_size;
    }

private:
    const T* m_symbol = 0;
};

#endif // ELFSYMBOLTABLEENTRY_IMPL_H
