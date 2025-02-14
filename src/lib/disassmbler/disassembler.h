/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
