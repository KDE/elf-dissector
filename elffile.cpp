#include "elffile.h"
#include "elfheader.h"
#include "elfsectionheader.h"

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
}

void ElfFile::parse()
{
    // TODO detect 32/64 versions
    m_header.reset(new ElfHeaderImpl<Elf64_Ehdr>(m_data));

//     qDebug() << Q_FUNC_INFO << m_header->e_ehsize << m_header->e_machine << m_header->e_shnum;

    for (int i = 0; i < m_header->sectionHeaderCount(); ++i) {
        // TODO 32/64 detection
        ElfSectionHeaderImpl<Elf64_Shdr> shdr(m_data + m_header->sectionHeaderTableOffset() + i * m_header->sectionHeaderEntrySize());
        qDebug() << i << "size:" << shdr.size() << "offset:" << shdr.offset() << stringTableEntry(shdr.name()) << shdr.type();
    }
}

const char* ElfFile::stringTableEntry(int index) const
{
    Elf64_Shdr *stringTableSection = reinterpret_cast<Elf64_Shdr*>(
        m_data + m_header->sectionHeaderTableOffset() + m_header->stringTableSectionHeader() * m_header->sectionHeaderEntrySize());
    return (const char*)(m_data + stringTableSection->sh_offset + index);
}
