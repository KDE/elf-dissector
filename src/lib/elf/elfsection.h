/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
