/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFGNUDEBUGLINKSECTION_H
#define ELFGNUDEBUGLINKSECTION_H

#include "elfsection.h"

#include <cstdint>

class QByteArray;

class ElfGnuDebugLinkSection : public ElfSection
{
public:
    explicit ElfGnuDebugLinkSection(ElfFile* file, ElfSectionHeader* shdr);
    ~ElfGnuDebugLinkSection();

    QByteArray fileName() const;
    uint32_t crc() const;
};

#endif // ELFGNUDEBUGLINKSECTION_H
