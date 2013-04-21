#include "elffile.h"
#include "elfheader.h"
#include "elfsectionheader_impl.h"
#include "elfstringtablesection.h"
#include "elfsymboltablesection_impl.h"

#include <QDebug>

#include <cassert>
#include <elf.h>

ElfFile::ElfFile(const QString& fileName) : m_file(fileName), m_data(nullptr)
{
    // TODO error handling
    m_file.open(QFile::ReadOnly);
    m_data = m_file.map(0, m_file.size());
    Q_ASSERT(m_data);

    parse();
}

ElfFile::~ElfFile()
{
    m_sectionHeaders.clear();
    m_sections.clear();
}

QString ElfFile::displayName() const
{
    return m_file.fileName();
}

uint64_t ElfFile::size() const
{
    return m_file.size();
}

const unsigned char* ElfFile::rawData() const
{
    return m_data;
}

int ElfFile::type() const
{
    // TODO throw exception
    assert(m_data);
    assert(m_file.size() > EI_CLASS);
    assert(m_data[EI_CLASS] == ELFCLASS32 || m_data[EI_CLASS] == ELFCLASS64);
    return m_data[EI_CLASS];
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
    parseHeader();
    parseSections();
}

void ElfFile::parseHeader()
{
    switch (type()) {
        case ELFCLASS32:
            m_header.reset(new ElfHeaderImpl<Elf32_Ehdr>(m_data));
            break;
        case ELFCLASS64:
            m_header.reset(new ElfHeaderImpl<Elf64_Ehdr>(m_data));
            break;
        default:
            assert(false); // TODO throw exception
    }
}

void ElfFile::parseSections()
{
    assert(m_header.get());

    m_sectionHeaders.reserve(m_header->sectionHeaderCount());
    m_sections.resize(m_header->sectionHeaderCount());

    // pass 1: create sections
    for (int i = 0; i < m_header->sectionHeaderCount(); ++i) {
        // TODO 32/64 detection
        ElfSectionHeader::Ptr shdr(new ElfSectionHeaderImpl<Elf64_Shdr>(this, i));
        m_sectionHeaders.push_back(shdr);

        ElfSection::Ptr section;
        switch (shdr->type()) {
            case SHT_STRTAB:
                section.reset(new ElfStringTableSection(this, shdr));
                break;
            case SHT_SYMTAB:
            case SHT_DYNSYM:
                if (type() == ELFCLASS32)
                    section.reset(new ElfSymbolTableSectionImpl<Elf32_Sym>(this, shdr));
                else if (type() == ELFCLASS64)
                    section.reset(new ElfSymbolTableSectionImpl<Elf64_Sym>(this, shdr));
                break;
            default:
                section.reset(new ElfSection(this, shdr));
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
}
