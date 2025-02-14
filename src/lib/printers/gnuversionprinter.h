/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef GNUVERSIONPRINTER_H
#define GNUVERSIONPRINTER_H

#include <cstdint>

class QByteArray;

/** Pretty printers related to GNU version definitions. */
namespace GnuVersionPrinter
{
    QByteArray versionFlags(uint16_t versionFlags);
}

#endif // GNUVERSIONPRINTER_H
