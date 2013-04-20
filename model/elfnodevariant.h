#ifndef ELFNODEVARIANT_H
#define ELFNODEVARIANT_H

struct ElfNodeVariant
{
    enum Type {
        Invalid,
        FileSet,
        File,
        Section,
        SymbolTableSection,
        SymbolTableEntry
    };

    template <typename T> T* value() const
    {
        return static_cast<T*>(payload);
    }

    void *payload = 0;
    Type type = Invalid;
};

#endif