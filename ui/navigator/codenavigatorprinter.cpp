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

#include "codenavigatorprinter.h"
#include "codenavigator.h"

#include <dwarf/dwarfdie.h>
#include <dwarf.h>

#include <QFileInfo>
#include <QUrl>

QString CodeNavigatorPrinter::sourceLocationRichText(DwarfDie* die)
{
    QString s;
    if (!die)
        return s;

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
        s += url.toEncoded();
        s += QLatin1String("\">");
    }
    s += fileName;
    if (lineNum > 0)
        s += ':' + QString::number(lineNum);
    if (hasCodeNavigation) {
        s += QLatin1String("</a>");
    }
    s += QLatin1String("<br/>");

    return s;
}
