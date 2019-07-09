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
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef ELFFILE_H
#define ELFFILE_H

#include "elfsectionheader.h"
#include "elfsection.h"
#include "elfdynamicsection.h"
#include "elfreverserelocator.h"

#include <QFile>
#include <QMetaType>
#include <QVector>

#include <memory>

class DwarfInfo;
class ElfHashSection;
class ElfHeader;
class ElfSymbolTableSection;
class ElfSegmentHeader;

/** Represents a ELF file. */
class ElfFile
{
public:
    explicit ElfFile(const QString &fileName);
    ElfFile(const ElfFile &other) = delete;
    /** Closes the file. */
    ~ElfFile();

    ElfFile& operator=(const ElfFile &other) = delete;

    /** Open the file and parse its content. Must be called before the file can be used. */
    bool open(QIODevice::OpenMode openMode);
    void close();


    /** Returns @c true if the file could be loaded and is parsed correctly. */
    bool isValid() const;

    /** Returns a user readable label for this file. */
    QString displayName() const;
    /** Returns the full path of this file. */
    QString fileName() const;
    /** Returns the size of the entire file. */
    uint64_t size() const;

    /** Returns a pointer to the raw ELF data. */
    unsigned char* rawData() const;

    /** ELF class type (32/64 bit). */
    int type() const;
    /** Returns the numbers of bytes needed to store an address. */
    int addressSize() const;
    /** Endianess. */
    int byteOrder() const;
    /** OS ABI. */
    uint8_t osAbi() const;

    /** Returns the ELF header. */
    ElfHeader* header() const;

    /** Returns the number of sections.
     *  Use this rather than header()->sectionHeaderCount() to include sections merged
     *  from separate debug files.
     */
    int sectionCount() const;
    /** Returns a list of all available section headers. */
    QVector<ElfSectionHeader*> sectionHeaders() const;
    /** Returns the section at index @p index. */
    template <typename T>
    inline T* section(int index) const
    {
        return dynamic_cast<T*>(m_sections.at(index));
    }
    /** Finds a section by type. */
    int indexOfSection(uint32_t type) const;
    /** Finds a section by name. */
    int indexOfSection(const char* name) const;
    /** Finds the section containing @p virtAddr. */
    int indexOfSectionWithVirtualAddress(uint64_t virtAddr) const;

    /** Returns the dynamic section. */
    ElfDynamicSection* dynamicSection() const;
    /** Returns .symtab if present, .dynsym otherwise. */
    ElfSymbolTableSection* symbolTable() const;
    /** Returns a symbol table hash section for fast lookup. */
    ElfHashSection* hash() const;
    /** Reverse relocation lookup. */
    const ElfReverseRelocator* reverseRelocator() const;

    /** Returns the build-id, if present. */
    QByteArray buildId() const;

    /** Sets the path to a separate file containing the DWARF debug information. */
    void setSeparateDebugFile(const QString &fileName);
    /** Returns the separate debug file, if present. */
    ElfFile* separateDebugFile() const;
    /** Returns @c true if this is a separate debug file. */
    bool isSeparateDebugFile() const;
    /** Returns the file with the actual content if this is a separate debug file. */
    ElfFile* contentFile() const;

    /** DWARF debug information, if present. */
    DwarfInfo* dwarfInfo() const;

    /** Returns a lost of all available segment headers. */
    QVector<ElfSegmentHeader*> segmentHeaders() const;

private:
    void parse();
    void parseHeader();
    void parseSections();
    void parseSection(uint16_t index);
    void parseSegments();

private:
    QFile m_file;
    uchar *m_data;
    std::unique_ptr<ElfHeader> m_header;
    QVector<ElfSectionHeader*> m_sectionHeaders;
    QVector<ElfSection*> m_sections;
    ElfDynamicSection* m_dynamicSection = nullptr;
    ElfHashSection* m_hashSection = nullptr;
    ElfReverseRelocator m_reverseReloc;
    std::unique_ptr<ElfFile> m_separateDebugFile;
    ElfFile *m_contentFile = nullptr; // the counter part for a separate debug file
    DwarfInfo *m_dwarfInfo = nullptr;
    QVector<ElfSegmentHeader*> m_segmentHeaders;
};

Q_DECLARE_METATYPE(ElfFile*)

#endif // ELFFILE_H
