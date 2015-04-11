/*
    Copyright (C) 2013-2014 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "elffile.h"
#include "elfheader.h"
#include "elfsectionheader_impl.h"
#include "elfstringtablesection.h"
#include "elfsymboltablesection_impl.h"
#include "elfdynamicsection_impl.h"

#include <dwarf/dwarfinfo.h>

#include <QDebug>

#include <cassert>
#include <elf.h>

ElfFile::ElfFile(const QString& fileName) : m_file(fileName), m_data(nullptr)
{
    // TODO error handling
    if (!m_file.open(QFile::ReadOnly)) {
        qCritical() << m_file.errorString() << fileName;
    }
    m_data = m_file.map(0, m_file.size());
    Q_ASSERT(m_data);

    parse();
}

ElfFile::~ElfFile()
{
    delete m_dwarfInfo;
    m_sectionHeaders.clear();
    qDeleteAll(m_sections);
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

int ElfFile::byteOrder() const
{
    assert(m_data);
    assert(m_file.size() > EI_DATA);
    return m_data[EI_DATA];
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

    if (indexOfSection(".debug_info") >= 0)
        m_dwarfInfo = new DwarfInfo(this);
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
        ElfSectionHeader::Ptr shdr;
        switch(type()) {
            case ELFCLASS32:
                shdr.reset(new ElfSectionHeaderImpl<Elf32_Shdr>(this, i));
                break;
            case ELFCLASS64:
                shdr.reset(new ElfSectionHeaderImpl<Elf64_Shdr>(this, i));
                break;
            default:
                assert(false);
        }
        m_sectionHeaders.push_back(shdr);

        ElfSection* section = nullptr;
        switch (shdr->type()) {
            case SHT_STRTAB:
                section = new ElfStringTableSection(this, shdr);
                break;
            case SHT_SYMTAB:
            case SHT_DYNSYM:
                if (type() == ELFCLASS32)
                    section = new ElfSymbolTableSectionImpl<Elf32_Sym>(this, shdr);
                else if (type() == ELFCLASS64)
                    section = new ElfSymbolTableSectionImpl<Elf64_Sym>(this, shdr);
                break;
            case SHT_DYNAMIC:
                if (type() == ELFCLASS32)
                    section = new ElfDynamicSectionImpl<Elf32_Dyn>(this, shdr);
                else if (type() == ELFCLASS64)
                    section = new ElfDynamicSectionImpl<Elf64_Dyn>(this, shdr);
                break;
            default:
                section = new ElfSection(this, shdr);
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

int ElfFile::indexOfSection(const char* name) const
{
    for (int i = 0; i < m_sectionHeaders.size(); ++i) {
        const auto hdr = m_sectionHeaders.at(i);
        if (qstrcmp(name, hdr->name()) == 0)
            return i;
    }
    return -1;
}

ElfDynamicSection* ElfFile::dynamicSection() const
{
    for (int i = 0; i < header()->sectionHeaderCount(); ++i) {
        if (m_sections.at(i)->header()->type() == SHT_DYNAMIC)
            return section<ElfDynamicSection>(i);
    }

    return nullptr;
}

DwarfInfo* ElfFile::dwarfInfo() const
{
    return m_dwarfInfo;
}
