/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFRELOCATIONENTRY_H
#define ELFRELOCATIONENTRY_H

#include <QtGlobal>
#include <cstdint>

class ElfRelocationSection;
class ElfSymbolTableEntry;

class ElfRelocationEntry
{
public:
    ElfRelocationEntry();
    ElfRelocationEntry(const ElfRelocationEntry&);
    explicit ElfRelocationEntry(const ElfRelocationSection *section, uint64_t index, bool withAddend);
    ~ElfRelocationEntry();

    ElfRelocationEntry& operator=(const ElfRelocationEntry&);

    const ElfRelocationSection* relocationTable() const;

    uint64_t offset() const;
    uint32_t symbolIndex() const;
    uint32_t type() const;
    uint64_t addend() const;

    /** Symbol table entry referenced from this relocation, can be @c nullptr. */
    ElfSymbolTableEntry* symbol() const;

    /** Returns the address the relocation actually points too in the end.
     *  How this is computed depends on the type of relocation and the platform.
     */
    uint64_t relocationTarget() const;

private:
    template <typename T> const T* entry() const;
    bool is64() const;

    const ElfRelocationSection *m_section;
    uint64_t m_index:63;
    uint64_t m_withAddend:1;
};

Q_DECLARE_TYPEINFO(ElfRelocationEntry, Q_MOVABLE_TYPE);

#endif // ELFRELOCATIONENTRY_H
