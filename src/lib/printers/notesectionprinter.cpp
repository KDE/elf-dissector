/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "notesectionprinter.h"
#include <elf/elfnoteentry.h>

#include <QByteArray>

#include <elf.h>

#include <cassert>

struct NoteType {
    uint32_t type;
    const char* name;
};

#define NT(type) { NT_ ## type, #type }

static const NoteType note_types[] {
    NT(GNU_ABI_TAG),
    NT(GNU_HWCAP),
    NT(GNU_BUILD_ID),
    NT(GNU_GOLD_VERSION)
};

#undef NT

static const int note_types_size = sizeof(note_types) / sizeof(NoteType);

#ifdef Q_OS_FREEBSD
// FreeBSD elf-common.h has no ELF_NOTE_OS_* and those are strings, anyway

static const NoteType os_types[] {
    { 0, ELF_NOTE_FREEBSD },
    // TODO: is that a sensible value?
    // TODO: there's also NETBSD, SOLARIS and GNU
};


#else
#define OS(type) { ELF_NOTE_OS_ ## type, #type }

static const NoteType os_types[] {
    OS(LINUX),
    OS(GNU),
    OS(SOLARIS2),
    OS(FREEBSD)
};

#undef OS
#endif

static const int os_types_size = sizeof(note_types) / sizeof(NoteType);

static const NoteType* noteType(uint32_t type, const NoteType *table, int size)
{
    for (int i = 0; i < size; ++i) {
        if (table[i].type == type)
            return &table[i];
    }
    return nullptr;
}

const char* NoteSectionPrinter::typeDisplayString(ElfNoteEntry* entry)
{
    if (!entry->isGNUVendorNote())
        return entry->name();

    const auto nt = noteType(entry->type(), note_types, note_types_size);
    if (!nt)
        return entry->name();
    return nt->name;
}

QByteArray NoteSectionPrinter::abi(ElfNoteEntry* entry)
{
    assert(entry->isGNUVendorNote());
    assert(entry->type() == NT_GNU_ABI_TAG);
    assert(entry->descriptionSize() >= 16);
    QByteArray r;

    struct ABIDesc {
        uint32_t os;
        uint32_t major;
        uint32_t minor;
        uint32_t patch;
    };
    const ABIDesc *abiDesc = reinterpret_cast<const ABIDesc*>(entry->descriptionData());

    const auto osType = noteType(abiDesc->os, os_types, os_types_size);
    if (osType) {
        r += osType->name;
    } else {
        r += "Unknown OS";
    }

    r += ' ';
    r += QByteArray::number(abiDesc->major);
    r += '.';
    r += QByteArray::number(abiDesc->minor);
    r += '.';
    r += QByteArray::number(abiDesc->patch);

    return r;
}
