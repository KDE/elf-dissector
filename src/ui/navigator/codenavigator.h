/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
