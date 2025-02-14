/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "symbolprinter.h"
#include "printerutils_p.h"

#include <elf.h>

static const LookupTableEntry<uint8_t> bind_type_table[] {
    { STB_LOCAL, "local" },
    { STB_GLOBAL, "global" },
    { STB_WEAK, "weak" },
    { STB_GNU_UNIQUE, "GNU unique" }
};

static const LookupTableEntry<uint8_t> symbol_type_table[] {
    { STT_NOTYPE, "unspecified" },
    { STT_OBJECT, "data object" },
    { STT_FUNC, "code object" },
    { STT_SECTION, "section" },
    { STT_FILE, "file name" },
    { STT_COMMON, "common data object" },
    { STT_TLS, "thread-local data object" },
    { STT_GNU_IFUNC, "GNU indirect code object" }
};

static const LookupTableEntry<uint8_t> visibility_type_table[] {
    { STV_DEFAULT, "default" },
    { STV_INTERNAL, "internal" },
    { STV_HIDDEN, "hidden" },
    { STV_PROTECTED, "protected" }
};


QByteArray SymbolPrinter::bindType(uint8_t type)
{
    return lookupLabel(type, bind_type_table);
}

QByteArray SymbolPrinter::symbolType(uint8_t type)
{
    return lookupLabel(type, symbol_type_table);
}

QByteArray SymbolPrinter::visibility(uint8_t type)
{
    return lookupLabel(type, visibility_type_table);
}
