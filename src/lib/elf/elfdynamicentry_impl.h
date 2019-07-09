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

#ifndef ELFDYNAMICENTRY_IMPL_H
#define ELFDYNAMICENTRY_IMPL_H

#include "elfdynamicentry.h"

template <typename T>
class ElfDynamicEntryImpl : public ElfDynamicEntry
{
public:
    explicit inline ElfDynamicEntryImpl(const ElfDynamicSection *section, uint32_t index) :
        ElfDynamicEntry(section),
        m_entry(reinterpret_cast<T*>(section->rawData() + index * section->header()->entrySize()))
    {
    }

    int64_t tag() const override
    {
        return m_entry->d_tag;
    }

    uint64_t value() const override
    {
        return m_entry->d_un.d_val;
    }

    void setValue(uint64_t value) override
    {
        m_entry->d_un.d_val = value;
    }

    uint64_t pointer() const override
    {
        return m_entry->d_un.d_ptr;
    }

private:
    T* m_entry = 0;

};

#endif
