#ifndef ELFNODEVARIANT_H
#define ELFNODEVARIANT_H

struct ElfNodeVariant
{
    enum Type {
        Invalid,
        File,
        Section,
        SymbolTableSection
    };

    template <typename T> T* value() const
    {
        return static_cast<T*>(payload);
    }

    void *payload = 0;
    Type type = Invalid;
};

#endif