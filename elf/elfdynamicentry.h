#ifndef ELFDYNAMICENTRY_H
#define ELFDYNAMICENTRY_H

#include <cstdint>
#include <memory>

class ElfDynamicSection;

class QString;

/** Entry in the dynamic section. */
class ElfDynamicEntry
{
public:
    typedef std::shared_ptr<ElfDynamicEntry> Ptr;

    ElfDynamicEntry(const ElfDynamicEntry &other) = delete;
    virtual ~ElfDynamicEntry();
    ElfDynamicEntry& operator=(const ElfDynamicEntry &other) = delete;

    /** The section this entry belongs to. */
    const ElfDynamicSection* dynamicSection() const;

    /** Human readable tag name. */
    QString tagName() const;

    /** Returns whether value() is an index into the string table. */
    bool isStringValue() const;
    /** Returns the string value for this entry. */
    const char* stringValue() const;

    virtual int64_t tag() const = 0;
    virtual uint64_t value() const = 0;
    virtual uint64_t pointer() const = 0;

protected:
    explicit ElfDynamicEntry(const ElfDynamicSection *section);
    const ElfDynamicSection *m_section;
};

#endif // ELFDYNAMICENTRY_H
