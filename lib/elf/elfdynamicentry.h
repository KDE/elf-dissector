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

#ifndef ELFDYNAMICENTRY_H
#define ELFDYNAMICENTRY_H

#include <cstdint>

class ElfDynamicSection;

class QString;

/** Entry in the dynamic section. */
class ElfDynamicEntry
{
public:
    ElfDynamicEntry(const ElfDynamicEntry &other) = delete;
    virtual ~ElfDynamicEntry();
    ElfDynamicEntry& operator=(const ElfDynamicEntry &other) = delete;

    /** The section this entry belongs to. */
    const ElfDynamicSection* dynamicSection() const;

    /** Human readable tag name. */
    QString tagName() const;

    /** Returns whether value() is an index into the string table. */
    bool isStringValue() const;
    /** Returns the string value for this entry. */
    const char* stringValue() const;

    /** Returns whether the value of this entry is an address (ie. pointer() returns something valid). */
    bool isAddress() const;

    virtual int64_t tag() const = 0;
    virtual uint64_t value() const = 0;
    /** Changes the value for this entry. Note that this actually writes to the file, assuming
     *  it's written in write mode.
     */
    virtual void setValue(uint64_t value) = 0;
    virtual uint64_t pointer() const = 0;

protected:
    explicit ElfDynamicEntry(const ElfDynamicSection *section);
    const ElfDynamicSection *m_section;
};

#endif // ELFDYNAMICENTRY_H
