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

#include "elfnoteentry.h"
#include "elfnotesection.h"
#include "elffile.h"

#include <elf.h>

ElfNoteEntry::ElfNoteEntry(const ElfNoteSection* section) :
    m_section(section)
{
}

ElfNoteEntry::~ElfNoteEntry()
{
}

const ElfNoteSection* ElfNoteEntry::section() const
{
    return m_section;
}

bool ElfNoteEntry::isGNUVendorNote() const
{
    return strcmp(name(), ELF_NOTE_GNU) == 0;
}
