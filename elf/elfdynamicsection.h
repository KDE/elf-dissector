#ifndef ELFDYNAMICSECTION_H
#define ELFDYNAMICSECTION_H

#include "elfarraysection.h"
#include "elfdynamicentry.h"

class ElfDynamicSection : public ElfArraySection<ElfDynamicEntry>
{
public:
    explicit ElfDynamicSection(ElfFile* file, const ElfSectionHeader::Ptr& shdr);
};

#endif // ELFDYNAMICSECTION_H
