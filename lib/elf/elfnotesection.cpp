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
