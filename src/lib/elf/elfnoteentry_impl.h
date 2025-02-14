/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFNOTEENTRY_IMPL_H
#define ELFNOTEENTRY_IMPL_H

#include "elfnoteentry.h"

template <typename T>
class ElfNoteEntryImpl : public ElfNoteEntry
{
public:
    explicit ElfNoteEntryImpl(const ElfNoteSection* section, uint64_t offset) :
        ElfNoteEntry(section),
        m_note(reinterpret_cast<const T*>(section->rawData() + offset))
    {
    }

    uint64_t type() const final override
    {
        return m_note->n_type;
    }

    uint64_t size() const final override
    {
        const uint64_t nameszSize = sizeof(((T*)nullptr)->n_namesz);
        return alignTo(nameSize(), nameszSize) + alignTo(descriptionSize(), nameszSize) + sizeof(T);
    }

    const char* name() const final override
    {
        return reinterpret_cast<const char*>(m_note) + sizeof(T);
    }

    uint64_t descriptionSize() const final override
    {
        return m_note->n_descsz;
    }

    const char* descriptionData() const final override
    {
        return reinterpret_cast<const char*>(m_note) + sizeof(T) + alignTo(nameSize(), sizeof(((T*)nullptr)->n_namesz));
    }

protected:
    uint64_t nameSize() const final override
    {
        return m_note->n_namesz;
    }

private:
    static uint64_t alignTo(uint64_t value, uint64_t size)
    {
        const auto r = value % size;
        if (!r)
            return value;
        return value + size - r;
    }

    const T * const m_note;
};

#endif
