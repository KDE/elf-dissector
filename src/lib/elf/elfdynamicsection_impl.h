/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFDYNAMICSECTION_IMPL_H
#define ELFDYNAMICSECTION_IMPL_H

#include "elfdynamicsection.h"
#include "elfdynamicentry_impl.h"

template <typename T>
class ElfDynamicSectionImpl : public ElfDynamicSection
{
public:
    explicit inline ElfDynamicSectionImpl(ElfFile *file, ElfSectionHeader *shdr) : ElfDynamicSection(file, shdr)
    {
        parse();
    }

    inline ElfDynamicEntry* createEntry(uint32_t index) const override
    {
        return new ElfDynamicEntryImpl<T>(this, index);
    }
};

#endif // ELFDYNAMICSECTION_IMPL_H
