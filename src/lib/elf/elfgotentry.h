/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFGOTENTRY_H
#define ELFGOTENTRY_H

#include <QtGlobal>

class ElfGotSection;
class ElfRelocationEntry;

/** Entry in the global offset table. */
class ElfGotEntry
{
public:
    ElfGotEntry();
    explicit ElfGotEntry(ElfGotSection *section, uint64_t index);

    ElfGotSection *section() const;
    uint64_t index() const;
    uint64_t address() const;

    /** Relocation information for this GOT entry, can be @c nullptr. */
    ElfRelocationEntry* relocation() const;

private:
    ElfGotSection *m_section;
    uint64_t m_index;
};

Q_DECLARE_TYPEINFO(ElfGotEntry, Q_MOVABLE_TYPE);

#endif // ELFGOTENTRY_H
