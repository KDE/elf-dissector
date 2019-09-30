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

#include "dynamicsectionprinter.h"
#include "printerutils_p.h"

#include <QByteArray>

#include <elf.h>

static const LookupTableEntry<uint64_t> dt_flag_infos[] {
    { DF_ORIGIN, "object may use DF_ORIGIN" },
    { DF_SYMBOLIC, "symbol resolutions starts here" },
    { DF_TEXTREL, "object contains text relocations" },
    { DF_BIND_NOW, "no lazy binding for this object" },
    { DF_STATIC_TLS, "module uses the static TLS model" }
};

static const LookupTableEntry<uint64_t> dt_flag_1_infos[] {
    { DF_1_NOW, "set RTLD_NOW for this object" },
    { DF_1_GLOBAL, "set RTLD_GLOBAL for this object" },
    { DF_1_GROUP, "set RTLD_GROUP for this object" },
    { DF_1_NODELETE, "set RTLD_NODELETE for this object" },
    { DF_1_LOADFLTR, "trigger filtee loading at runtime" },
    { DF_1_INITFIRST, "set RTLD_INITFIRST for this object" },
    { DF_1_NOOPEN, "set RTLD_NOOPEN for this object" },
    { DF_1_ORIGIN, "$ORIGIN must be handled" },
    { DF_1_DIRECT, "direct binding enabled" },
    { DF_1_TRANS, "DF_1_TRANS" },
    { DF_1_INTERPOSE, "object is used to interpose" },
    { DF_1_NODEFLIB, "ignore default lib search path" },
    { DF_1_NODUMP, "object cannot be dldump'ed" },
    { DF_1_CONFALT, "configuration alternative created" },
    { DF_1_ENDFILTEE, "filtee terminates filters search" },
    { DF_1_DISPRELDNE, "disp reloc applied at build time" },
    { DF_1_DISPRELPND, "disp reloc applied at run-time" },
#if __GLIBC_PREREQ(2, 17)
    { DF_1_NODIRECT, "object has no-direct binding" },
    { DF_1_IGNMULDEF, "DF_1_IGNMULDEF" },
    { DF_1_NOKSYMS, "DF_1_NOKSYMS" },
    { DF_1_NOHDR, "DF_!_NOHDR" },
    { DF_1_EDITED, "Object is modified after built" },
    { DF_1_NORELOC, "DF_1_NORELOC" },
    { DF_1_SYMINTPOSE, "Object has individual interposers" },
    { DF_1_GLOBAUDIT, "Global auditing required" },
    { DF_1_SINGLETON, "Singleton symbols are used" }
#endif
};

namespace DynamicSectionPrinter {

QByteArray flagsToDescriptions(uint64_t flags)
{
    return lookupFlags(flags, dt_flag_infos);
}

QByteArray flags1ToDescriptions(uint64_t flags)
{
    return lookupFlags(flags, dt_flag_1_infos);
}

}
