/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
