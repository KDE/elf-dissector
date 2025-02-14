/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DYNAMICSECTIONPRINTER_H
#define DYNAMICSECTIONPRINTER_H

#include <cstdint>

class QByteArray;

/** Pretty printers for the .dynamic section. */
namespace DynamicSectionPrinter
{
    /** Comma separated human readable descriptions of the flags set in @p flags. */
    QByteArray flagsToDescriptions(uint64_t flags);
    /** Comma separated human readable descriptions of the flags set in @p flags. */
    QByteArray flags1ToDescriptions(uint64_t flags);
}

#endif // DYNAMICSECTIONPRINTER_H
