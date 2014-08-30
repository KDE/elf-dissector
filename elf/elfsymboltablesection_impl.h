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

#ifndef ELFSYMBOLTABLESECTION_IMPL_H
#define ELFSYMBOLTABLESECTION_IMPL_H

#include "elfsymboltablesection.h"
#include "elfsymboltableentry_impl.h"

template <typename T>
class ElfSymbolTableSectionImpl : public ElfSymbolTableSection
{
public:
    explicit inline ElfSymbolTableSectionImpl(ElfFile *file, const ElfSectionHeader::Ptr &shdr) : ElfSymbolTableSection(file, shdr)
    {
        parse();
    }

    inline ElfSymbolTableEntry::Ptr createEntry(uint32_t index) const override
    {
        return ElfSymbolTableEntry::Ptr(new ElfSymbolTableEntryImpl<T>(this, index));
    }
};

#endif // ELFSYMBOLTABLESECTION_IMPL_H
