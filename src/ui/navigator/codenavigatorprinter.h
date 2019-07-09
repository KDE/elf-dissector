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

#ifndef CODENAVIGATORPRINTER_H
#define CODENAVIGATORPRINTER_H

class DwarfDie;
class QString;

/** Utility functions for code navigation that depend on other parts of the code.
 *  Located separately, so that the code navigator as such is easy to re-use.
 */
namespace CodeNavigatorPrinter
{
    /** Pretty-printed source location with link to code navigation if file exists. */
    QString sourceLocationRichText(DwarfDie *die);
}

#endif // CODENAVIGATORPRINTER_H
