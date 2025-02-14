/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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

    QList<ElfFile*> m_files;
    QList<QByteArray> m_baseSearchPaths;
    QList<QByteArray> m_ldLibraryPaths;

    QList<QString> m_globalDebugSearchPath;
};

#endif // ELFFILESET_H
