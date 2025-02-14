/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFNODEVARIANT_H
#define ELFNODEVARIANT_H

struct ElfNodeVariant
{
    enum Type {
        Invalid,
        FileSet,
        File,
        Section,
        SymbolTableSection,
        SymbolTableEntry,
        DebugLinkSection,
        DynamicSection,
        DynamicEntry,
        GotSection,
        GotEntry,
        HashSection,
        NoteSection,
        NoteEntry,
        PltSection,
        PltEntry,
        RelocationSection,
        RelocationEntry,
        VersionDefinitionSection,
        VersionDefinitionEntry,
        VersionDefinitionAuxiliaryEntry,
        VersionRequirementsSection,
        VersionRequirementEntry,
        VersionRequirementAuxiliaryEntry,
        DwarfInfo,
        DwarfDie
    };

    template <typename T> T* value() const
    {
        return static_cast<T*>(payload);
    }

    void *payload = nullptr;
    Type type = Invalid;
};

#endif
