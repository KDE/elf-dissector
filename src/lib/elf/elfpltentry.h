/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFPLTENTRY_H
#define ELFPLTENTRY_H

#include <QtGlobal>

class ElfPltSection;
class ElfGotEntry;

class ElfPltEntry
{
public:
    ElfPltEntry() = default;
    ElfPltEntry(const ElfPltEntry&);
    explicit ElfPltEntry(ElfPltSection* section, uint64_t index);
    ~ElfPltEntry() = default;

    ElfPltEntry& operator=(const ElfPltEntry&);

    ElfPltSection* section() const;
    uint64_t index() const;
    const uchar* rawData() const;
    uint64_t size() const;

    /** The GOT entry used by this PLT entry. */
    ElfGotEntry* gotEntry() const;

private:
    ElfPltSection *m_section = nullptr;
    uint64_t m_index = 0;
};

Q_DECLARE_TYPEINFO(ElfPltEntry, Q_MOVABLE_TYPE);

#endif // ELFPLTENTRY_H
