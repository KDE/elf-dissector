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

#include "gnuversionprinter.h"
#include "printerutils_p.h"

#include <QByteArray>

#include <elf.h>

static const LookupTableEntry<uint16_t> version_flags_table[] = {
    { VER_FLG_BASE, "version definition of file itself" },
    { VER_FLG_WEAK, "weak version identifier" }
};

QByteArray GnuVersionPrinter::versionFlags(uint16_t versionFlags)
{
    return lookupFlags(versionFlags, version_flags_table);
}
