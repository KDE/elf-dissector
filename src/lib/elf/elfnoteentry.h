/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFNOTEENTRY_H
#define ELFNOTEENTRY_H

#include <cstdint>

class ElfNoteSection;

/** Entries in .note sections. */
class ElfNoteEntry
{
public:
    ElfNoteEntry(const ElfNoteEntry&) = delete;
    virtual ~ElfNoteEntry();

    ElfNoteEntry& operator=(const ElfNoteEntry&) = delete;

    /** Note type. */
    virtual uint64_t type() const = 0;

    /** Returns the section this entry belongs to. */
    const ElfNoteSection *section() const;
    /** Size of this entry, including padding, ie. use this one to find the next one. */
    virtual uint64_t size() const = 0;

    /** Name of this note entry. */
    virtual const char* name() const = 0;

    /** Size of the description field. */
    virtual uint64_t descriptionSize() const = 0;
    /** Data of the description field. */
    virtual const char* descriptionData() const = 0;

    /** Check if this is a GNU vendor note. */
    bool isGNUVendorNote() const;

protected:
    virtual uint64_t nameSize() const = 0;
    explicit ElfNoteEntry(const ElfNoteSection *section);

private:
    const ElfNoteSection * const m_section;

};

#endif // ELFNOTEENTRY_H
