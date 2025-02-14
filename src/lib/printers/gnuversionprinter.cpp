/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
