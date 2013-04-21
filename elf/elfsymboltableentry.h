#ifndef ELFSYMBOLTABLEENTRY_H
#define ELFSYMBOLTABLEENTRY_H

#include <cstdint>
#include <memory>

class ElfSymbolTableSection;
class QByteArray;

/** Symbol table entry. */
class ElfSymbolTableEntry
{
public:
    typedef std::shared_ptr<ElfSymbolTableEntry> Ptr;

    ElfSymbolTableEntry(const ElfSymbolTableEntry &other) = delete;
    virtual ~ElfSymbolTableEntry();
    ElfSymbolTableEntry& operator=(const ElfSymbolTableEntry &other) = delete;

    virtual uint32_t nameIndex() const = 0;
    virtual uint8_t visibility() const = 0;
    virtual uint16_t sectionIndex() const = 0;
    virtual uint64_t value() const = 0;
    virtual uint64_t size() const = 0;

    /** Bind type. */
    uint8_t bindType() const;
    /** Symbol type. */
    uint8_t type() const;

    /** Mangled name from string table. */
    const char* name() const;
    /** Demangled name for user display. */
    QByteArray prettyName() const;

protected:
    explicit ElfSymbolTableEntry(const ElfSymbolTableSection *section);
    virtual uint8_t info() const = 0;
    const ElfSymbolTableSection *m_section;

};

#endif // ELFSYMBOLTABLEENTRY_H
