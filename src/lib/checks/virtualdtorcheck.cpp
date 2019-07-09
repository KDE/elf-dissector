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

#include "virtualdtorcheck.h"

#include <elf/elffileset.h>
#include <dwarf/dwarfinfo.h>
#include <dwarf/dwarfdie.h>
#include <dwarf/dwarfcudie.h>
#include <dwarf/dwarftypes.h>

#include <dwarf.h>


#include <algorithm>
#include <iostream>

void VirtualDtorCheck::findImplicitVirtualDtors(ElfFileSet* fileSet)
{
    for (int i = 0; i < fileSet->size(); ++i) {
        const auto file = fileSet->file(i);
        if (!file->dwarfInfo())
            continue;
        foreach (const auto die, file->dwarfInfo()->compilationUnits())
            findImplicitVirtualDtors(die);
    }

    // implicit virtual dtors in implementation files are not a problem
    m_results.erase(
        std::remove_if(m_results.begin(), m_results.end(), [](const Result &res) {
            return res.sourceFilePath.endsWith(QLatin1String(".cpp"))
                || res.sourceFilePath.endsWith(QLatin1String(".c"))
                || res.sourceFilePath.endsWith(QLatin1String(".cxx"));
        }), m_results.end()
    );
}

void VirtualDtorCheck::findImplicitVirtualDtors(DwarfDie* die)
{
    const bool isCandidate =
        die->tag() == DW_TAG_subprogram &&
        die->attribute(DW_AT_external).toBool() &&
        die->attribute(DW_AT_declaration).toBool() &&
        die->attribute(DW_AT_artificial).toBool() &&
        die->attribute(DW_AT_virtuality).value<DwarfVirtuality>() == DwarfVirtuality::Virtual &&
        die->name().startsWith('~');

    if (isCandidate) {
        const auto fullName = die->fullyQualifiedName();
        const auto it = std::find_if(m_results.begin(), m_results.end(), [&fullName](const Result& res) {
            return res.fullName == fullName;
        });
        const auto *typeDie = die->attribute(DW_AT_containing_type).value<DwarfDie*>();
        if (it == m_results.end()) {
            const Result res = {
                fullName,
                typeDie ? typeDie->sourceFilePath() : QString(),
                typeDie ? typeDie->attribute(DW_AT_decl_line).toInt() : 0
            };
            m_results.push_back(res);
        } else if ((*it).sourceFilePath.isEmpty() && typeDie) {
            (*it).sourceFilePath = typeDie->sourceFilePath();
            (*it).lineNumber = typeDie->attribute(DW_AT_decl_line).toInt();
        }
    }

    const auto children = die->children();
    for (const auto child : children) {
        if (child->tag() != DW_TAG_subprogram &&
            child->tag() != DW_TAG_class_type &&
            child->tag() != DW_TAG_structure_type &&
            child->tag() != DW_TAG_namespace)
            continue;
        findImplicitVirtualDtors(child);
    }
}

void VirtualDtorCheck::printResults() const
{
    for (const auto &res : m_results) {
        std::cout << res.fullName.constData() << " at " << qPrintable(res.sourceFilePath);
        if (res.lineNumber)
            std::cout << ':' << res.lineNumber;
        std::cout << std::endl;
    }
}

void VirtualDtorCheck::clear()
{
    m_results.clear();
}

const QVector< VirtualDtorCheck::Result >& VirtualDtorCheck::results() const
{
    return m_results;
}
