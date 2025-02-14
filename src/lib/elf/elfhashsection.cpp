/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
