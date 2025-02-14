/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "config-elf-dissector.h"
#include "codenavigatorprinter.h"
#include "codenavigator.h"

#if HAVE_DWARF
#include <dwarf/dwarfdie.h>
#include <dwarf.h>
#endif

#include <QFileInfo>
#include <QUrl>

using namespace Qt::Literals;

QString CodeNavigatorPrinter::sourceLocationRichText(DwarfDie* die)
{
    QString s;
    if (!die)
        return s;

#if HAVE_DWARF
    const auto fileName = die->sourceFilePath();
    if (fileName.isEmpty())
        return s;

    const auto lineNum = die->attribute(DW_AT_decl_line).toInt();
    const auto hasCodeNavigation = CodeNavigator::isValid() && QFileInfo(fileName).isAbsolute();

    s += QLatin1String("Source location: ");
    if (hasCodeNavigation) {
        QUrl url;
        url.setScheme(QStringLiteral("code"));
        url.setPath(fileName);
        url.setFragment(QString::number(lineNum));
        s += QLatin1String("<a href=\"");
        s += QString::fromUtf8(url.toEncoded());
        s += QLatin1String("\">");
    }
    s += fileName;
    if (lineNum > 0)
        s += ':'_L1 + QString::number(lineNum);
    if (hasCodeNavigation) {
        s += QLatin1String("</a>");
    }
    s += QLatin1String("<br/>");
#endif

    return s;
}
