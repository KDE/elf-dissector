/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
