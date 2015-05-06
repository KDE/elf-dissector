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
#include "elfgnusymbolversiontable.h"
#include "elfgnusymbolversiondefinitionssection.h"
#include "elfgnusymbolversionrequirementssection.h"
#include "elfnotesection.h"
#include "elfrelocationsection.h"

#include <dwarf/dwarfinfo.h>

#include <QDebug>
#include <QFileInfo>

#include <cassert>
#include <elf.h>

struct ElfFileException {};

ElfFile::ElfFile(const QString& fileName) : m_data(nullptr)
{
    open(fileName);
}

ElfFile::~ElfFile()
{
    close();
}

bool ElfFile::isValid() const
{
    return m_data;
}

QString ElfFile::displayName() const
{
    if (dynamicSection() && !dynamicSection()->soName().isEmpty())
        return dynamicSection()->soName();
    return QFileInfo(m_file.fileName()).fileName();
}

QString ElfFile::fileName() const
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
    assert(isValid());
    return m_data[EI_CLASS];
}

int ElfFile::addressSize() const
{
    assert(isValid());
    return type() == ELFCLASS32 ? 4 : 8;
}

int ElfFile::byteOrder() const
{
    assert(isValid());
    return m_data[EI_DATA];
}

uint8_t ElfFile::osAbi() const
{
    return m_data[EI_OSABI];
}

ElfHeader* ElfFile::header() const
{
    return m_header.get();
}

QVector<ElfSectionHeader*> ElfFile::sectionHeaders() const
{
    return m_sectionHeaders;
}

void ElfFile::open(const QString &fileName)
{
    m_file.setFileName(fileName);
    if (!m_file.open(QFile::ReadOnly)) {
        qCritical() << m_file.errorString() << fileName;
        return;
    }
    m_data = m_file.map(0, m_file.size());
    if (!m_data) {
        close();
        return;
    }

    try {
        parse();
    } catch (const ElfFileException&) {
        close();
    }
}

void ElfFile::close()
{
    delete m_dwarfInfo;
    qDeleteAll(m_sectionHeaders);
    qDeleteAll(m_sections);
    m_file.close();
    m_data = nullptr;
}

void ElfFile::parse()
{
    static_assert(EV_CURRENT == 1, "ELF version changed");
    if (m_file.size() <= EI_NIDENT || strncmp(reinterpret_cast<const char*>(m_data), ELFMAG, SELFMAG) != 0 || m_data[EI_VERSION] != EV_CURRENT)
        throw ElfFileException();

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
            throw ElfFileException();
    }
}

void ElfFile::parseSections()
{
    assert(m_header.get());

    m_sectionHeaders.reserve(m_header->sectionHeaderCount());
    m_sections.resize(m_header->sectionHeaderCount());

    // pass 1: create sections
    for (int i = 0; i < m_header->sectionHeaderCount(); ++i) {
        ElfSectionHeader* shdr = nullptr;
        switch(type()) {
            case ELFCLASS32:
                shdr = new ElfSectionHeaderImpl<Elf32_Shdr>(this, i);
                break;
            case ELFCLASS64:
                shdr = new ElfSectionHeaderImpl<Elf64_Shdr>(this, i);
                break;
            default:
                throw ElfFileException();
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
            case SHT_REL:
            case SHT_RELA:
                section = new ElfRelocationSection(this, shdr);
                break;
            case SHT_NOTE:
                section = new ElfNoteSection(this, shdr);
                break;
            case SHT_GNU_versym:
                section = new ElfGNUSymbolVersionTable(this, shdr);
                break;
            case SHT_GNU_verdef:
                section = new ElfGNUSymbolVersionDefinitionsSection(this, shdr);
                break;
            case SHT_GNU_verneed:
                section = new ElfGNUSymbolVersionRequirementsSection(this, shdr);
                break;
            default:
                section = new ElfSection(this, shdr);
                break;
        }
        m_sections[i] = section;
    }

    // pass 2: set section links
    for (const auto shdr : m_sectionHeaders) {
        if (shdr->link()) {
            m_sections[shdr->sectionIndex()]->setLinkedSection(m_sections[shdr->link()]);
        }
    }

    // pass 3: stuff that requires the full setup for parsing
    // TODO can probably be done more efficient with on-demand parsing in those places
    for (auto section : m_sections) {
        switch (section->header()->type()) {
            case SHT_GNU_verdef:
                dynamic_cast<ElfGNUSymbolVersionDefinitionsSection*>(section)->parse();
                continue;
            case SHT_GNU_verneed:
                dynamic_cast<ElfGNUSymbolVersionRequirementsSection*>(section)->parse();
                continue;
        }
    }
}

int ElfFile::indexOfSection(uint32_t type) const
{
    for (int i = 0; i < m_sectionHeaders.size(); ++i) {
        if (m_sectionHeaders.at(i)->type() == type)
            return i;
    }
    return -1;
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
    const auto index = indexOfSection(SHT_DYNAMIC);
    if (index < 0)
        return nullptr;
    return section<ElfDynamicSection>(index);
}

ElfSymbolTableSection* ElfFile::symbolTable() const
{
    auto index = indexOfSection(SHT_SYMTAB);
    if (index < 0)
        index = indexOfSection(SHT_DYNSYM);
    if (index < 0)
        return nullptr;
    return section<ElfSymbolTableSection>(index);
}

DwarfInfo* ElfFile::dwarfInfo() const
{
    return m_dwarfInfo;
}
