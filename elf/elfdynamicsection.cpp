#include "elfdynamicsection.h"

ElfDynamicSection::ElfDynamicSection(ElfFile* file, const ElfSectionHeader::Ptr& shdr): ElfArraySection< ElfDynamicEntry >(file, shdr)
{
}
