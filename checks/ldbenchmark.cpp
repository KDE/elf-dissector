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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ldbenchmark.h"

#include <elf/elffile.h>
#include <elf/elffileset.h>

#include <QProcess>
#include <QStringList>

void LDBenchmark::measureFileSet(ElfFileSet* fileSet)
{
    QStringList args;
    args.reserve(fileSet->size() + 2);
    args.push_back("RTLD_LAZY");
    for (int i = fileSet->size() - 1; i >= 0; --i) {
        args.push_back(fileSet->file(i)->displayName());
    }

    QProcess proc;
    proc.setProcessChannelMode(QProcess::ForwardedChannels);
    proc.start("ldbenchmark-runner", args); // TODO find in libexec
    proc.waitForFinished();
}
