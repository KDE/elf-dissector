/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFSTRINGTABLESECTION_H
#define ELFSTRINGTABLESECTION_H

#include "elfsection.h"

/** String table section. */
class ElfStringTableSection : public ElfSection
{
public:
    explicit ElfStringTableSection(ElfFile *file, ElfSectionHeader *shdr);

    /** Returns the string at @p index. */
    const char* string(uint32_t index) const;
};

#endif // ELFSTRINGTABLESECTION_H
