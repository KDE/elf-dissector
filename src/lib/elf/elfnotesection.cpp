/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "elfnotesection.h"
#include "elfnoteentry_impl.h"
#include "elffile.h"

#include <elf.h>

ElfNoteSection::ElfNoteSection(ElfFile* file, ElfSectionHeader* shdr):
    ElfSection(file, shdr)
{
    uint64_t offset = 0;
    ElfNoteEntry *entry = nullptr;
    do {
        if (file->type() == ELFCLASS32) {
            entry = new ElfNoteEntryImpl<Elf32_Nhdr>(this, offset);
        } else {
            // ### somehow the note sections in 64bit files are also using Elf32_Nhdr!?
//             entry = new ElfNoteEntryImpl<Elf64_Nhdr>(this, offset);
            entry = new ElfNoteEntryImpl<Elf32_Nhdr>(this, offset);
        }
        m_entries.push_back(entry);
        offset += entry->size();
    } while(offset < size() && entry->size() > 0);
}

ElfNoteSection::~ElfNoteSection()
{
    qDeleteAll(m_entries);
}

int ElfNoteSection::entryCount() const
{
    return m_entries.size();
}

ElfNoteEntry* ElfNoteSection::entry(int index) const
{
    return m_entries.at(index);
}
