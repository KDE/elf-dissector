/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
