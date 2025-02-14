/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFARRAYSECTION_H
#define ELFARRAYSECTION_H

#include "elfsection.h"

#include <QList>

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

    QList<T*> m_entries;
};

#endif // ELFARRAYSECTION_H
