#ifndef ELFDYNAMICSECTION_H
#define ELFDYNAMICSECTION_H

#include "elfarraysection.h"
#include "elfdynamicentry.h"

class ElfDynamicSection : public ElfArraySection<ElfDynamicEntry>
{
public:
    typedef std::shared_ptr<ElfDynamicSection> Ptr;

    explicit ElfDynamicSection(ElfFile* file, const ElfSectionHeader::Ptr& shdr);

    /** SO name. */
    QByteArray soName() const;
    /** RPATHs. */
    QVector<QByteArray> rpaths() const;
    /** RUNPATHs. */
    QVector<QByteArray> runpaths() const;
    /** Needed libraries. */
    QVector<QByteArray> neededLibraries() const;

private:
    QVector<QByteArray> stringList(int64_t tag) const;
};

#endif // ELFDYNAMICSECTION_H
