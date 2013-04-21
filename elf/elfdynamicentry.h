#ifndef ELFDYNAMICENTRY_H
#define ELFDYNAMICENTRY_H

#include <cstdint>
#include <memory>

class ElfDynamicSection;

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

    virtual int64_t tag() const = 0;
    virtual uint64_t value() const = 0;
    virtual uint64_t pointer() const = 0;

protected:
    explicit ElfDynamicEntry(const ElfDynamicSection *section);
    const ElfDynamicSection *m_section;
};

#endif // ELFDYNAMICENTRY_H
