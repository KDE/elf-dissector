#include "elffile.h"
#include "elfheader.h"
#include "elfsectionheader_impl.h"
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

QString ElfFile::displayName() const
{
    return m_file.fileName();
}

qint64 ElfFile::size() const
{
    return m_file.size();
}

const unsigned char* ElfFile::rawData() const
{
    return m_data;
}

ElfHeader* ElfFile::header() const
{
    return m_header.get();
}

QVector< ElfSectionHeader::Ptr > ElfFile::sectionHeaders()
{
    return m_sectionHeaders;
}

void ElfFile::parse()
{
    // TODO detect 32/64 versions
    m_header.reset(new ElfHeaderImpl<Elf64_Ehdr>(m_data));

//     qDebug() << Q_FUNC_INFO << m_header->e_ehsize << m_header->e_machine << m_header->e_shnum;

    m_sectionHeaders.reserve(m_header->sectionHeaderCount());
    m_sections.resize(m_header->sectionHeaderCount());

    // pass 1: create sections
    for (int i = 0; i < m_header->sectionHeaderCount(); ++i) {
        // TODO 32/64 detection
        ElfSectionHeader::Ptr shdr(new ElfSectionHeaderImpl<Elf64_Shdr>(this, i));
        m_sectionHeaders.push_back(shdr);

        ElfSection *section = 0;
        switch (shdr->type()) {
            case SHT_STRTAB:
                section = new ElfStringTableSection(m_data + shdr->sectionOffset(), shdr->size());
                break;
            case SHT_SYMTAB:
                section = new ElfSymbolTableSectionImpl<Elf64_Sym>(m_data + shdr->sectionOffset(), shdr->size());
                break;
            default:
                section = new ElfSection(m_data + shdr->sectionOffset(), shdr->size());
                break;
        }
        m_sections[i] = section;
    }

    // pass 2: set section links
    for (const ElfSectionHeader::Ptr &shdr : m_sectionHeaders) {
        if (shdr->link()) {
            m_sections[shdr->sectionIndex()]->setLinkedSection(m_sections[shdr->link()]);
        }
    }

    // pass 3: debug output
    for (const ElfSectionHeader::Ptr &shdr : m_sectionHeaders) {
        qDebug() << shdr->sectionIndex() << "size:" << shdr->size() << "offset:" << shdr->sectionOffset() << stringTableEntry(shdr->name()) << shdr->type();
        if (shdr->type() == SHT_SYMTAB) {
            ElfSymbolTableSection* symtab = static_cast<ElfSymbolTableSection*>(m_sections.at(shdr->sectionIndex()));
            for (unsigned int j = 0; j < (shdr->size() / shdr->entrySize()); ++j) {
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
