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

#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <cstdint>

class QString;

class ElfFile;
class ElfPltEntry;
class ElfSection;
class ElfSymbolTableEntry;
class ElfGotEntry;

class DwarfLine;

class Disassembler
{
public:
    Disassembler();
    Disassembler(const Disassembler&) = delete;
    virtual ~Disassembler();

    Disassembler& operator=(const Disassembler&) = delete;

    QString disassemble(ElfSymbolTableEntry *entry);
    QString disassemble(ElfSection *section);
    QString disassemble(ElfPltEntry *entry);

    // internal
    ElfFile* file() const;
    uint64_t baseAddress() const;
    void printAddress(uint64_t addr, QString *s) const;

    /** Pretty-print symbol name, override for adding navigation links etc. */
    virtual QString printSymbol(ElfSymbolTableEntry *entry) const;
    /** ditto, for .got entries. */
    virtual QString printGotEntry(ElfGotEntry *entry) const;
    /** ditto, for .plt entries. */
    virtual QString printPltEntry(ElfPltEntry *entry) const;

private:
    QString disassemble(const unsigned char* data, uint64_t size);
    QString disassembleBinutils(const unsigned char* data, uint64_t size);
    QString disassembleCapstone(const unsigned char* data, uint64_t size);

    DwarfLine lineForAddress(uint64_t addr) const;
    QString printSourceLine(DwarfLine line) const;

    ElfFile *m_file = nullptr;
    uint64_t m_baseAddress = 0;
};

#endif // DISASSEMBLER_H
