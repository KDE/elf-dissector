/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef NOTESECTIONPRINTER_H
#define NOTESECTIONPRINTER_H

class ElfNoteEntry;
class QByteArray;

/** Pretty printers for .note section content. */
namespace NoteSectionPrinter
{
    const char* typeDisplayString(ElfNoteEntry *entry);
    QByteArray abi(ElfNoteEntry *entry);
}

#endif // NOTESECTIONPRINTER_H
