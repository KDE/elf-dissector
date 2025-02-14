/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "elfsegmentheader.h"

ElfSegmentHeader::ElfSegmentHeader(ElfFile* file) : m_file(file)
{
}

ElfSegmentHeader::~ElfSegmentHeader() = default;

ElfFile* ElfSegmentHeader::file() const
{
    return m_file;
}
