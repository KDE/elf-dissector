#ifndef ELFARRAYSECTION_H
#define ELFARRAYSECTION_H

#include "elfsection.h"

#include <QVector>

/** Base class for sections with array content. */
template <typename T>
class ElfArraySection : public ElfSection
{
public:
    inline ~ElfArraySection() override
    {
        m_entries.clear();
    }

    inline typename T::Ptr entry(uint32_t index) const
    {
        return m_entries.at(index);
    }

protected:
    explicit inline ElfArraySection(ElfFile *file, const typename ElfSectionHeader::Ptr &shdr) : ElfSection(file, shdr) {}
    virtual typename T::Ptr createEntry(uint32_t index) const = 0;
    /** Must be called from sub-class ctor. */
    inline void parse()
    {
        m_entries.reserve(header()->entryCount());
        for (uint32_t index = 0; index < header()->entryCount(); ++index)
            m_entries.push_back(createEntry(index));
    }

private:
    QVector<typename T::Ptr> m_entries;
};

#endif // ELFARRAYSECTION_H
