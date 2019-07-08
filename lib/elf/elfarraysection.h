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

#ifndef ELFARRAYSECTION_H
#define ELFARRAYSECTION_H

#include "elfsection.h"

#include <QVector>

/** Base class for sections with array content. */
template <typename T>
class ElfArraySection : public ElfSection
{
public:
    inline ~ElfArraySection() override
    {
        qDeleteAll(m_entries);
    }

    inline T* entry(uint32_t index) const
    {
        return m_entries.at(index);
    }

protected:
    explicit inline ElfArraySection(ElfFile *file, ElfSectionHeader* &shdr) : ElfSection(file, shdr) {}
    virtual T* createEntry(uint32_t index) const = 0;
    /** Must be called from sub-class ctor. */
    inline void parse()
    {
        m_entries.reserve(header()->entryCount());
        for (uint32_t index = 0; index < header()->entryCount(); ++index)
            m_entries.push_back(createEntry(index));
    }

    QVector<T*> m_entries;
};

#endif // ELFARRAYSECTION_H
