/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFNODEVISITOR_H
#define ELFNODEVISITOR_H

#include "config-elf-dissector.h"

#include <elf/elfheader.h>
#include <elf/elfsymboltablesection.h>
#include <elf/elfdynamicsection.h>
#include <elf/elfgnudebuglinksection.h>
#include <elf/elfgnusymbolversiondefinition.h>
#include <elf/elfgnusymbolversiondefinitionssection.h>
#include <elf/elfgnusymbolversionrequirementssection.h>
#include <elf/elfgotsection.h>
#include <elf/elfhashsection.h>
#include <elf/elfgnuhashsection.h>
#include <elf/elfnotesection.h>
#include <elf/elfpltsection.h>
#include <elf/elfrelocationsection.h>
#include <elf/elfrelocationentry.h>

#if HAVE_DWARF
#include <dwarf/dwarfdie.h>
#include <dwarf/dwarfinfo.h>
#endif

#include "elfnodevariant.h"

#include <qglobal.h>

#include <cassert>

class ElfDynamicEntry;
class ElfFile;
class ElfFileSet;
class ElfGNUSymbolVersionRequirementAuxiliaryEntry;
class ElfPltEntry;

template <typename T>
class ElfNodeVisitor
{
public:
    typedef T type;

    ElfNodeVisitor() = default;
    ElfNodeVisitor(const ElfNodeVisitor&) = delete;
    virtual ~ElfNodeVisitor() = default;
    ElfNodeVisitor& operator=(ElfNodeVisitor&) = delete;

    inline T visit(ElfNodeVariant *node, int arg = 0) const
    {
        switch (node->type) {
            case ElfNodeVariant::Invalid:
                assert(false);
                break;
            case ElfNodeVariant::FileSet:
                return doVisit(node->value<ElfFileSet>(), arg);
            case ElfNodeVariant::File:
                return doVisit(node->value<ElfFile>(), arg);
            case ElfNodeVariant::Section:
                return doVisit(node->value<ElfSection>(), arg);
            case ElfNodeVariant::SymbolTableSection:
                return doVisit(node->value<ElfSymbolTableSection>(), arg);
            case ElfNodeVariant::SymbolTableEntry:
                return doVisit(node->value<ElfSymbolTableEntry>(), arg);
            case ElfNodeVariant::DebugLinkSection:
                return doVisit(node->value<ElfGnuDebugLinkSection>(), arg);
            case ElfNodeVariant::DynamicSection:
                return doVisit(node->value<ElfDynamicSection>(), arg);
            case ElfNodeVariant::DynamicEntry:
                return doVisit(node->value<ElfDynamicEntry>(), arg);
            case ElfNodeVariant::GotSection:
                return doVisit(node->value<ElfGotSection>(), arg);
            case ElfNodeVariant::GotEntry:
                return doVisit(node->value<ElfGotEntry>(), arg);
            case ElfNodeVariant::HashSection:
                return doVisit(node->value<ElfHashSection>(), arg);
            case ElfNodeVariant::NoteSection:
                return doVisit(node->value<ElfNoteSection>(), arg);
            case ElfNodeVariant::NoteEntry:
                return doVisit(node->value<ElfNoteEntry>(), arg);
            case ElfNodeVariant::PltSection:
                return doVisit(node->value<ElfPltSection>(), arg);
            case ElfNodeVariant::PltEntry:
                return doVisit(node->value<ElfPltEntry>(), arg);
            case ElfNodeVariant::RelocationSection:
                return doVisit(node->value<ElfRelocationSection>(), arg);
            case ElfNodeVariant::RelocationEntry:
                return doVisit(node->value<ElfRelocationEntry>(), arg);
            case ElfNodeVariant::VersionDefinitionSection:
                return doVisit(node->value<ElfGNUSymbolVersionDefinitionsSection>(), arg);
            case ElfNodeVariant::VersionDefinitionEntry:
                return doVisit(node->value<ElfGNUSymbolVersionDefinition>(), arg);
            case ElfNodeVariant::VersionDefinitionAuxiliaryEntry:
                return doVisit(node->value<ElfGNUSymbolVersionDefinitionAuxiliaryEntry>(), arg);
            case ElfNodeVariant::VersionRequirementsSection:
                return doVisit(node->value<ElfGNUSymbolVersionRequirementsSection>(), arg);
            case ElfNodeVariant::VersionRequirementEntry:
                return doVisit(node->value<ElfGNUSymbolVersionRequirement>(), arg);
            case ElfNodeVariant::VersionRequirementAuxiliaryEntry:
                return doVisit(node->value<ElfGNUSymbolVersionRequirementAuxiliaryEntry>(), arg);
#if HAVE_DWARF
            case ElfNodeVariant::DwarfInfo:
                return doVisit(node->value<DwarfInfo>(), arg);
            case ElfNodeVariant::DwarfDie:
                return doVisit(node->value<DwarfDie>(), arg);
#endif
        }

        assert(false);
        Q_UNREACHABLE();
    }

protected:
    virtual T doVisit(ElfFileSet *fileSet, int arg) const
    {
        Q_UNUSED(fileSet);
        Q_UNUSED(arg);
        return T();
    }

    virtual T doVisit(ElfFile* file, int arg) const
    {
        Q_UNUSED(file);
        Q_UNUSED(arg);
        return T();
    }
    virtual T doVisit(ElfSection* section, int arg) const
    {
        Q_UNUSED(section);
        Q_UNUSED(arg);
        return T();
    }
    virtual T doVisit(ElfSymbolTableSection *symtab, int arg) const
    {
        return doVisit(static_cast<ElfSection*>(symtab), arg);
    }
    virtual T doVisit(ElfSymbolTableEntry *entry, int arg) const
    {
        Q_UNUSED(entry);
        Q_UNUSED(arg);
        return T();
    }
    virtual T doVisit(ElfGnuDebugLinkSection *section, int arg) const
    {
        return doVisit(static_cast<ElfSection*>(section), arg);
    }
    virtual T doVisit(ElfDynamicSection* section, int arg) const
    {
        return doVisit(static_cast<ElfSection*>(section), arg);
    }
    virtual T doVisit(ElfDynamicEntry*, int) const
    {
        return T();
    }
    virtual T doVisit(ElfHashSection *section, int arg) const
    {
        return doVisit(static_cast<ElfSection*>(section), arg);
    }
    virtual T doVisit(ElfGNUSymbolVersionDefinitionsSection *section, int arg) const
    {
        return doVisit(static_cast<ElfSection*>(section), arg);
    }
    virtual T doVisit(ElfGNUSymbolVersionDefinition*, int) const
    {
        return T();
    }
    virtual T doVisit(ElfGNUSymbolVersionDefinitionAuxiliaryEntry*, int) const
    {
        return T();
    }
    virtual T doVisit(ElfGNUSymbolVersionRequirementsSection *section, int arg) const
    {
        return doVisit(static_cast<ElfSection*>(section), arg);
    }
    virtual T doVisit(ElfGNUSymbolVersionRequirement*, int) const
    {
        return T();
    }
    virtual T doVisit(ElfGNUSymbolVersionRequirementAuxiliaryEntry*, int) const
    {
        return T();
    }
    virtual T doVisit(ElfGotSection *section, int arg) const
    {
        return doVisit(static_cast<ElfSection*>(section), arg);
    }
    virtual T doVisit(ElfGotEntry*, int) const
    {
        return T();
    }
    virtual T doVisit(ElfNoteSection* section, int arg) const
    {
        return doVisit(static_cast<ElfSection*>(section), arg);
    }
    virtual T doVisit(ElfNoteEntry*, int) const
    {
        return T();
    }
    virtual T doVisit(ElfPltSection *section, int arg) const
    {
        return doVisit(static_cast<ElfSection*>(section), arg);
    }
    virtual T doVisit(ElfPltEntry*, int) const
    {
        return T();
    }
    virtual T doVisit(ElfRelocationSection *section, int arg) const
    {
        return doVisit(static_cast<ElfSection*>(section), arg);
    }
    virtual T doVisit(ElfRelocationEntry*, int) const
    {
        return T();
    }
#if HAVE_DWARF
    virtual T doVisit(DwarfInfo*, int) const
    {
        return T();
    }
    virtual T doVisit(DwarfDie*, int) const
    {
       return T();
    }
#endif
};

#endif
