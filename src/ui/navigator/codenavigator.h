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

#ifndef CODENAVIGATOR_H
#define CODENAVIGATOR_H

class QAction;
class QString;
class QUrl;
class QWidget;

/** Navigate to a given code path in your favorite IDE. */
namespace CodeNavigator
{
    /** Returns @c true if we can do code navigation. */
    bool isValid();

    /** Open @p filePath, and navigate to line @p line if that's greater or equal to 0. */
    void goTo(const QString &filePath, int line = -1);
    /** URL-encoded form of the above. */
    void goTo(const QUrl &url);

    /** Returns a action for selecting your preferred IDE. */
    QAction* configMenu(QWidget *parent = nullptr);
}

#endif // CODENAVIGATOR_H
