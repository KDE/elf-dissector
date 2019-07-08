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

#ifndef ELFSECTION_H
#define ELFSECTION_H

#include "elfsectionheader.h"

#include <QMetaType>

#include <cstdint>

class ElfFile;

/** Base class for all ELF sections. */
class ElfSection
{
public:
    explicit ElfSection(ElfFile *file, ElfSectionHeader *shdr);
    ElfSection(const ElfSection &other) = delete;
    virtual ~ElfSection();
    ElfSection& operator=(const ElfSection &other) = delete;

    template <typename T>
    inline T* linkedSection() const
    {
        return dynamic_cast<T*>(m_linkedSection);
    }
    void setLinkedSection(ElfSection* linkedSection);

    /** Size of the section. */
    uint64_t size() const;
    /** Access to the raw data of the section. */
    unsigned char* rawData() const;

    /** The file this section belongs to. */
    ElfFile* file() const;
    /** Returns the corresponding section header. */
    ElfSectionHeader* header() const;

protected:
    ElfFile *m_file;
    ElfSectionHeader *m_sectionHeader;
    ElfSection *m_linkedSection;
};

Q_DECLARE_METATYPE(ElfSection*)

#endif // ELFSECTION_H
