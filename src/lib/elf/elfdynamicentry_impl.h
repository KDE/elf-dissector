/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
