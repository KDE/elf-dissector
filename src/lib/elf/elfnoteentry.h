/*
    Copyright (C) 2015 Volker Krause <vkrause@kde.org>

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
