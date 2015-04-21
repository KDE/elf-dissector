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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ELFRELOCATIONENTRY_H
#define ELFRELOCATIONENTRY_H

#include <cstdint>

class ElfRelocationSection;

class ElfRelocationEntry
{
public:
    ElfRelocationEntry(const ElfRelocationEntry&) = delete;
    virtual ~ElfRelocationEntry();
    ElfRelocationEntry& operator=(const ElfRelocationEntry&) = delete;

    const ElfRelocationSection* relocationTable() const;

    virtual uint64_t offset() const = 0;
    virtual uint32_t symbol() const = 0;
    virtual uint32_t type() const = 0;
    virtual uint64_t addend() const = 0;

protected:
    explicit ElfRelocationEntry(const ElfRelocationSection *section);
    const ElfRelocationSection * const m_section;

private:
};

#endif // ELFRELOCATIONENTRY_H
