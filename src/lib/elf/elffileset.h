/*
    Copyright (C) 2013-2014 Volker Krause <vkrause@kde.org>

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

#ifndef ELFFILESET_H
#define ELFFILESET_H

#include "elffile.h"

#include <QObject>

/** A set of ELF files. */
class ElfFileSet : public QObject
{
    Q_OBJECT
public:
    explicit ElfFileSet(QObject* parent = nullptr);
    ~ElfFileSet();

    int size() const;
    void addFile(const QString &fileName);

    ElfFile* file(int index) const;

    void topologicalSort();
private:
    void addFile(ElfFile* file);
    void parseLdConf();
    void parseLdConf(const QString &fileName);
    void findSeparateDebugFile(ElfFile *file) const;
    static bool isValidDebugLinkFile(const QString& fileName, uint32_t expectedCrc);

    QVector<ElfFile*> m_files;
    QVector<QByteArray> m_baseSearchPaths;
    QVector<QByteArray> m_ldLibraryPaths;

    QVector<QString> m_globalDebugSearchPath;
};

#endif // ELFFILESET_H
