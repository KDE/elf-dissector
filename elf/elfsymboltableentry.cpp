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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "elfsymboltableentry.h"
#include "elfsymboltablesection.h"
#include "elfstringtablesection.h"
#include "elffile.h"

#include <QByteArray>

#include <elf.h>

ElfSymbolTableEntry::ElfSymbolTableEntry(const ElfSymbolTableSection* section) :  m_section(section)
{
}

ElfSymbolTableEntry::~ElfSymbolTableEntry()
{
}

const ElfSymbolTableSection* ElfSymbolTableEntry::symbolTable() const
{
    return m_section;
}

const char* ElfSymbolTableEntry::name() const
{
    return m_section->linkedSection<ElfStringTableSection>()->string(nameIndex());
}

uint8_t ElfSymbolTableEntry::bindType() const
{
    // same as 64
    return ELF32_ST_BIND(info());
}

uint8_t ElfSymbolTableEntry::type() const
{
    // same as 64
    return ELF32_ST_TYPE(info());
}

uint8_t ElfSymbolTableEntry::visibility() const
{
    // same as 65
    return ELF32_ST_VISIBILITY(other());
}

const unsigned char* ElfSymbolTableEntry::data() const
{
    ElfSection::Ptr targetSection = m_section->file()->section<ElfSection>(sectionIndex());
    return targetSection->rawData() + value() - targetSection->header()->virtualAddress();
}
