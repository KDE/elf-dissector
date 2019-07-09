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
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
