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

#include "elfhashsection.h"

ElfHashSection::ElfHashSection(ElfFile* file, ElfSectionHeader* shdr) :
    ElfSection(file, shdr)
{
}

ElfHashSection::~ElfHashSection() = default;

int ElfHashSection::commonPrefixLength(const char* s1, const char* s2)
{
    int l = 0;
    while (*s1 && *s2 && *s1 == *s2) {
        ++l;
        ++s1;
        ++s2;
    }
    return l;
}
