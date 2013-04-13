#ifndef ELFFILE_H
#define ELFFILE_H

#include <QFile>
#include <QVector>

#include <memory>

class ElfHeader;
class ElfSection;

/** Represents a ELF file. */
class ElfFile
{
public:
    explicit ElfFile(const QString &fileName);
    ElfFile(const ElfFile &other) = delete;
    ~ElfFile();

    void parse();

    const char* stringTableEntry(int index) const;

private:
    QFile m_file;
    uchar *m_data;
    std::unique_ptr<ElfHeader> m_header;
    QVector<ElfSection*> m_sections;
};

#endif // ELFFILE_H
