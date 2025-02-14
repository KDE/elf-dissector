/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "elfstringtablesection.h"

#include <cassert>

ElfStringTableSection::ElfStringTableSection(ElfFile* file, ElfSectionHeader *shdr): ElfSection(file, shdr)
{
}

const char* ElfStringTableSection::string(uint32_t index) const
{
    assert(index < size());
    return (const char*)(rawData() + index);
}
