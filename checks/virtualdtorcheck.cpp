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

#include "virtualdtorcheck.h"

#include <elf/elffileset.h>
#include <dwarf/dwarfinfo.h>
#include <dwarf/dwarfdie.h>

#include <libdwarf/dwarf.h>

#include <QDebug>

#include <algorithm>
#include <iostream>

void VirtualDtorCheck::findImplicitVirtualDtors(ElfFileSet* fileSet)
{
    for (int i = 0; i < fileSet->size(); ++i) {
        const auto file = fileSet->file(i);
        if (!file->dwarfInfo())
            continue;
        for (const auto die : file->dwarfInfo()->compilationUnits())
            findImplicitVirtualDtors(die);
    }

    printResults();
}

void VirtualDtorCheck::findImplicitVirtualDtors(DwarfDie* die)
{
    const bool isCandidate =
        die->tag() == DW_TAG_subprogram &&
        die->attribute(DW_AT_external).toBool() &&
        die->attribute(DW_AT_declaration).toBool() &&
        die->attribute(DW_AT_artificial).toBool() &&
        die->attribute(DW_AT_virtuality).toString() == QLatin1String("DW_VIRTUALITY_virtual") && // TODO this should be an enum, not a stringq
        die->name().startsWith('~');

    if (isCandidate) {
        const auto fullName = die->name(); // TODO generate fully qualified name here!
        const auto it = std::find_if(m_results.begin(), m_results.end(), [&fullName](const Result& res) {
            return res.fullName == fullName;
        });
        const auto *typeDie = die->attribute(DW_AT_containing_type).value<DwarfDie*>();
        if (it == m_results.end()) {
            const Result res = { fullName, typeDie ? typeDie->sourceLocation() : QString()};
            m_results.push_back(res);
        } else if ((*it).sourceLocation.isEmpty()) {
            (*it).sourceLocation = typeDie ? typeDie->sourceLocation() : QString();
        }
    }

    // TODO can we abort traversal earlier?
    for (const auto child : die->children()) {
        findImplicitVirtualDtors(child);
    }
}

void VirtualDtorCheck::printResults()
{
    for (const auto &res : m_results)
        std::cout << res.fullName.constData() << " at " << qPrintable(res.sourceLocation) << std::endl;
}
