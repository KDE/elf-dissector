#include "elffile.h"
#include "elfheader.h"
#include "elfsectionheader.h"
#include "elfstringtablesection.h"
#include "elfsymboltablesection.h"

#include <QDebug>

#include <elf.h>

ElfFile::ElfFile(const QString& fileName) : m_file(fileName), m_data(nullptr)
{
    // TODO error handling
    m_file.open(QFile::ReadOnly);
    m_data = m_file.map(0, m_file.size());
    Q_ASSERT(m_data);
}

ElfFile::~ElfFile()
{
    qDeleteAll(m_sections);
}

void ElfFile::parse()
{
    // TODO detect 32/64 versions
    m_header.reset(new ElfHeaderImpl<Elf64_Ehdr>(m_data));

//     qDebug() << Q_FUNC_INFO << m_header->e_ehsize << m_header->e_machine << m_header->e_shnum;

    m_sections.resize(m_header->sectionHeaderCount());

    // pass 1: create sections
    for (int i = 0; i < m_header->sectionHeaderCount(); ++i) {
        // TODO 32/64 detection
        ElfSectionHeaderImpl<Elf64_Shdr> shdr(m_data + m_header->sectionHeaderTableOffset() + i * m_header->sectionHeaderEntrySize());
        ElfSection *section = 0;
        switch (shdr.type()) {
            case SHT_STRTAB:
                section = new ElfStringTableSection(m_data + shdr.offset(), shdr.size());
                break;
            case SHT_SYMTAB:
                section = new ElfSymbolTableSectionImpl<Elf64_Sym>(m_data + shdr.offset(), shdr.size());
                break;
            default:
                section = new ElfSection(m_data + shdr.offset(), shdr.size());
                break;
        }
        m_sections[i] = section;
    }

    // pass 2: set section links
    for (int i = 0; i < m_header->sectionHeaderCount(); ++i) {
        // TODO 32/64 detection
        ElfSectionHeaderImpl<Elf64_Shdr> shdr(m_data + m_header->sectionHeaderTableOffset() + i * m_header->sectionHeaderEntrySize());
        if (shdr.link()) {
            m_sections[i]->setLinkedSection(m_sections[shdr.link()]);
        }
    }

    // pass 3: debug output
    for (int i = 0; i < m_header->sectionHeaderCount(); ++i) {
        // TODO 32/64 detection
        ElfSectionHeaderImpl<Elf64_Shdr> shdr(m_data + m_header->sectionHeaderTableOffset() + i * m_header->sectionHeaderEntrySize());
        qDebug() << i << "size:" << shdr.size() << "offset:" << shdr.offset() << stringTableEntry(shdr.name()) << shdr.type();
        if (shdr.type() == SHT_SYMTAB) {
            ElfSymbolTableSection* symtab = static_cast<ElfSymbolTableSection*>(m_sections.at(i));
            for (unsigned int j = 0; j < (shdr.size() / shdr.entrySize()); ++j) {
                ElfSymbolTableSection::ElfSymbolTableEntry *entry = symtab->entry(j);
                qDebug() << j << symtab->linkedSection<ElfStringTableSection>()->string(entry->name()) << entry->size();
                delete entry;
            }
        }
    }
}

const char* ElfFile::stringTableEntry(int index) const
{
    Elf64_Shdr *stringTableSection = reinterpret_cast<Elf64_Shdr*>(
        m_data + m_header->sectionHeaderTableOffset() + m_header->stringTableSectionHeader() * m_header->sectionHeaderEntrySize());
    return (const char*)(m_data + stringTableSection->sh_offset + index);
}
