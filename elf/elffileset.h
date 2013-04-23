#ifndef ELFFILESET_H
#define ELFFILESET_H

#include "elffile.h"

#include <QObject>
#include <QStringList>

/** A set of ELF files. */
class ElfFileSet : public QObject
{
    Q_OBJECT
public:
    explicit ElfFileSet(QObject* parent = 0);
    ~ElfFileSet();

    int size() const;
    void addFile(const QString &fileName);

    ElfFile::Ptr file(int index) const;

private:
    QString findLibrary(const QString &name) const;

    QVector<ElfFile::Ptr> m_files;
    QStringList m_searchPaths;
};

#endif // ELFFILESET_H
