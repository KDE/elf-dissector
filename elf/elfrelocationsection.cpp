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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "elfrelocationsection.h"
#include "elfrelocationentry_impl.h"
#include "elffile.h"

#include <elf.h>

ElfRelocationSection::ElfRelocationSection(ElfFile* file, ElfSectionHeader* shdr) :
    ElfArraySection<ElfRelocationEntry>(file, shdr)
{
    parse();
}

ElfRelocationSection::~ElfRelocationSection() = default;

ElfRelocationEntry* ElfRelocationSection::createEntry(uint32_t index) const
{
    const auto elfType = file()->type();
    const auto secType = header()->type();

    if (elfType == ELFCLASS64 && secType == SHT_REL)
        return new ElfRelocationEntryImpl<Elf64_Rel>(this, index);
    if (elfType == ELFCLASS64 && secType == SHT_RELA)
        return new ElfRelocationEntryImplA<Elf64_Rela>(this, index);
    if (elfType == ELFCLASS32 && secType == SHT_REL)
        return new ElfRelocationEntryImpl<Elf32_Rel>(this, index);
    if (elfType == ELFCLASS32 && secType == SHT_RELA)
        return new ElfRelocationEntryImplA<Elf32_Rela>(this, index);

    Q_UNREACHABLE();
}
