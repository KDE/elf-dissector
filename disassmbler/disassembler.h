#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

class QString;

class ElfSymbolTableEntry;

class Disassembler
{
public:
    QString disassemble(ElfSymbolTableEntry *entry) const;
};

#endif // DISASSEMBLER_H
