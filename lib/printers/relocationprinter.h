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

#ifndef RELOCATIONPRINTER_H
#define RELOCATIONPRINTER_H

#include <cstdint>

class ElfRelocationEntry;
class QByteArray;

/** Pretty printers for relocation information. */
namespace RelocationPrinter
{
    QByteArray label(ElfRelocationEntry* entry);
    QByteArray label(uint16_t machine, uint32_t type);
    QByteArray description(ElfRelocationEntry* entry);
}

#endif // RELOCATIONPRINTER_H
