#ifndef ELFFILESET_H
#define ELFFILESET_H

#include "elffile.h"

#include <QObject>

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
    QVector<ElfFile::Ptr> m_files;
};

#endif // ELFFILESET_H
