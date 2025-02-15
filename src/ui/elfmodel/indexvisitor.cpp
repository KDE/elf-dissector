/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "indexvisitor.h"

#include <elf/elffile.h>
#include <elf/elffileset.h>
#include <elf/elfgnusymbolversionrequirement.h>
#if HAVE_DWARF
#include <dwarf/dwarfcudie.h>
#endif

#include <elf.h>

IndexVisitor::type IndexVisitor::doVisit(ElfFileSet* fileSet, int row) const
{
    return qMakePair<void*, ElfNodeVariant::Type>(fileSet->file(row), ElfNodeVariant::File);
}

IndexVisitor::type IndexVisitor::doVisit(ElfFile* file, int row) const
{
    ElfSection *section = file->section<ElfSection>(row);
    void *internalPointer = section;
    ElfNodeVariant::Type type;
    switch (section->header()->type()) {
        case SHT_SYMTAB:
        case SHT_DYNSYM:
            type = ElfNodeVariant::SymbolTableSection;
            break;
        case SHT_DYNAMIC:
            type = ElfNodeVariant::DynamicSection;
            break;
        case SHT_NOTE:
            type = ElfNodeVariant::NoteSection;
            break;
        case SHT_REL:
        case SHT_RELA:
            type = ElfNodeVariant::RelocationSection;
            break;
        case SHT_GNU_verdef:
            type = ElfNodeVariant::VersionDefinitionSection;
            break;
        case SHT_GNU_verneed:
            type = ElfNodeVariant::VersionRequirementsSection;
            break;
        case SHT_HASH:
        case SHT_GNU_HASH:
            type = ElfNodeVariant::HashSection;
            break;
        case SHT_PROGBITS:
            if (strcmp(section->header()->name(), ".plt") == 0) {
                type = ElfNodeVariant::PltSection;
                break;
            } else if ((section->header()->flags() & SHF_WRITE) && strncmp(section->header()->name(), ".got", 4) == 0) {
                type = ElfNodeVariant::GotSection;
                break;
            } else if (strcmp(section->header()->name(), ".gnu_debuglink") == 0) {
                type = ElfNodeVariant::DebugLinkSection;
                break;
            }
            // else fallthrough
        default:
            if (strcmp(section->header()->name(), ".debug_info") == 0) {
#if HAVE_DWARF
                type = ElfNodeVariant::DwarfInfo;
                internalPointer = file->dwarfInfo();
#else
                type = ElfNodeVariant::Section;
#endif
            } else {
                type = ElfNodeVariant::Section;
            }
    }
    return qMakePair(internalPointer, type);
}

IndexVisitor::type IndexVisitor::doVisit(ElfSymbolTableSection* symtab, int row) const
{
    const auto entry = symtab->entry(row);
    return qMakePair<void*, ElfNodeVariant::Type>(entry, ElfNodeVariant::SymbolTableEntry);
}

IndexVisitor::type IndexVisitor::doVisit(ElfDynamicSection* section, int row) const
{
    const auto entry = section->entry(row);
    return qMakePair<void*, ElfNodeVariant::Type>(entry, ElfNodeVariant::DynamicEntry);
}

IndexVisitor::type IndexVisitor::doVisit(ElfGNUSymbolVersionDefinitionsSection *section, int row) const
{
    const auto entry = section->definition(row);
    return qMakePair<void*, ElfNodeVariant::Type>(entry, ElfNodeVariant::VersionDefinitionEntry);
}

IndexVisitor::type IndexVisitor::doVisit(ElfGNUSymbolVersionDefinition *verDef, int row) const
{
    const auto auxEntry = verDef->auxiliaryEntry(row);
    return qMakePair<void*, ElfNodeVariant::Type>(auxEntry, ElfNodeVariant::VersionDefinitionAuxiliaryEntry);
}

IndexVisitor::type IndexVisitor::doVisit(ElfGNUSymbolVersionRequirementsSection *section, int row) const
{
    const auto entry = section->requirement(row);
    return qMakePair<void*, ElfNodeVariant::Type>(entry, ElfNodeVariant::VersionRequirementEntry);
}

IndexVisitor::type IndexVisitor::doVisit(ElfGNUSymbolVersionRequirement *verNeed, int row) const
{
    const auto auxEntry = verNeed->auxiliaryEntry(row);
    return qMakePair<void*, ElfNodeVariant::Type>(auxEntry, ElfNodeVariant::VersionRequirementAuxiliaryEntry);
}

IndexVisitor::type IndexVisitor::doVisit(ElfGotSection *section, int row) const
{
    const auto entry = section->entry(row);
    return qMakePair<void*, ElfNodeVariant::Type>(entry, ElfNodeVariant::GotEntry);
}

IndexVisitor::type IndexVisitor::doVisit(ElfNoteSection *section, int row) const
{
    const auto entry = section->entry(row);
    return qMakePair<void*, ElfNodeVariant::Type>(entry, ElfNodeVariant::NoteEntry);
}

IndexVisitor::type IndexVisitor::doVisit(ElfPltSection *section, int row) const
{
    const auto entry = section->entry(row);
    return qMakePair<void*, ElfNodeVariant::Type>(entry, ElfNodeVariant::PltEntry);
}

IndexVisitor::type IndexVisitor::doVisit(ElfRelocationSection* section, int row) const
{
    const auto entry = section->entry(row);
    return qMakePair<void*, ElfNodeVariant::Type>(entry, ElfNodeVariant::RelocationEntry);
}

#if HAVE_DWARF
IndexVisitor::type IndexVisitor::doVisit(DwarfInfo* info, int row) const
{
    auto cuDie = info->compilationUnits().at(row);
    return qMakePair<void*, ElfNodeVariant::Type>(cuDie, ElfNodeVariant::DwarfDie);
}

QPair< void*, ElfNodeVariant::Type > IndexVisitor::doVisit(DwarfDie* die, int row) const
{
    DwarfDie *childDie = die->children().at(row);
    return qMakePair<void*, ElfNodeVariant::Type>(childDie, ElfNodeVariant::DwarfDie);
}
#endif
