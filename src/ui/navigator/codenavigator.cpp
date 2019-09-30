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

#include "codenavigator.h"

#include <QAction>
#include <QInputDialog>
#include <QMenu>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>
#include <QUrl>

struct ide_settings_t {
    const char* const app;
    const char* const args;
    const char* const name;
    const char* const icon;
};

static const ide_settings_t ide_settings[] {
    { "kdevelop", "%f:%l", QT_TRANSLATE_NOOP("CodeNavigator", "KDevelop"), "kdevelop" },
    { "kate", "%f --line %l", QT_TRANSLATE_NOOP("CodeNavigator", "Kate"), "kate" },
    { "kwrite", "%f --line %l", QT_TRANSLATE_NOOP("CodeNavigator", "KWrite"), nullptr },
    { "qtcreator", "%f", QT_TRANSLATE_NOOP("CodeNavigator", "Qt Creator"), nullptr }
};

static const int ide_settings_size = sizeof(ide_settings) / sizeof(ide_settings_t);


bool CodeNavigator::isValid()
{
    return true; // TODO
}

void CodeNavigator::goTo(const QString& filePath, int line)
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("CodeNavigator"));
    const auto ideIdx = settings.value(QStringLiteral("IDE"), -1).toInt();

    QString command;
    if (ideIdx >= 0 && ideIdx < ide_settings_size) {
        command += ide_settings[ideIdx].app;
        command += ' ';
        command += ide_settings[ideIdx].args;
    } else {
        command = settings.value(QStringLiteral("CustomCommand")).toString();
    }

    command.replace(QLatin1String("%f"), filePath);
    command.replace(QLatin1String("%l"), QString::number(std::max(0, line)));

    if (!command.isEmpty())
        QProcess::startDetached(command);
}

void CodeNavigator::goTo(const QUrl& url)
{
    goTo(url.path(), url.fragment().toInt());
}

QAction* CodeNavigator::configMenu(QWidget *parent)
{
    static QAction *configAction = nullptr;
    if (!configAction) {
        configAction = new QAction(QIcon::fromTheme(QStringLiteral("applications-development")), QObject::tr("Code Navigation"), parent);
        auto menu = new QMenu(parent);
        auto group = new QActionGroup(parent);
        group->setExclusive(true);

        QSettings settings;
        settings.beginGroup(QStringLiteral("CodeNavigator"));
        const auto currentIdx = settings.value(QStringLiteral("IDE"), -1).toInt();

        for (int i = 0; i < ide_settings_size; ++i) {
            auto action = new QAction(menu);
            action->setText(QObject::tr(ide_settings[i].name));
            if (ide_settings[i].icon)
                action->setIcon(QIcon::fromTheme(ide_settings[i].icon));
            action->setCheckable(true);
            action->setChecked(currentIdx == i);
            action->setData(i);
            action->setEnabled(!QStandardPaths::findExecutable(ide_settings[i].app).isEmpty());
            group->addAction(action);
            menu->addAction(action);
        }
        menu->addSeparator();

        auto action = new QAction(menu);
        action->setText(QObject::tr("Custom..."));
        action->setCheckable(true);
        action->setChecked(currentIdx == -1);
        group->addAction(action);
        menu->addAction(action);

        QObject::connect(group, &QActionGroup::triggered, [parent](QAction *action) {
            QSettings settings;
            settings.beginGroup(QStringLiteral("CodeNavigator"));

            if (!action->data().isValid()) {
                const auto customCmd = QInputDialog::getText(
                    parent, QObject::tr("Custom Code Navigation"),
                    QObject::tr("Specify command to use for code navigation, '%f' will be replaced by the file name, '%l' by the line number."),
                    QLineEdit::Normal, settings.value(QStringLiteral("CustomCommand")).toString()
                );
                if (!customCmd.isEmpty()) {
                    settings.setValue(QStringLiteral("CustomCommand"), customCmd);
                    settings.setValue(QStringLiteral("IDE"), -1);
                }
                return;
            }

            const auto defaultIdx = action->data().toInt();
            settings.setValue(QStringLiteral("IDE"), defaultIdx);
        });

        configAction->setMenu(menu);
    }

    return configAction;
}
