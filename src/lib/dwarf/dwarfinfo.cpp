/*
    Copyright (C) 2014-2015 Volker Krause <vkrause@kde.org>

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

#include "dwarfinfo.h"
#include "dwarfcudie.h"
#include "dwarfaddressranges.h"
#include "dwarfranges.h"

#include <QDebug>

#include <dwarf.h>
#include <libdwarf.h>

#include <elf.h>

#include <type_traits>

class DwarfInfoPrivate {
public:
    DwarfInfoPrivate(DwarfInfo* qq);
    ~DwarfInfoPrivate();

    void scanCompilationUnits();
    DwarfDie *dieForMangledSymbolRecursive(const QByteArray &symbol, DwarfDie *die) const;

    ElfFile *elfFile = nullptr;
    QVector<DwarfCuDie*> compilationUnits;
#ifdef Q_OS_FREEBSD
    Dwarf_Obj_Access_Interface objAccessIface{};
    Dwarf_Obj_Access_Methods objAccessMethods{};
#else
    Dwarf_Obj_Access_Interface_a_s objAccessIface{};
    Dwarf_Obj_Access_Methods_a objAccessMethods{};
#endif

    Dwarf_Debug dbg{};

    DwarfInfo *q;
    DwarfAddressRanges *aranges = nullptr;

    bool isValid;
};


#ifdef Q_OS_FREEBSD
static Dwarf_Endianness callback_get_byte_order(void *obj)
{
    const DwarfInfoPrivate *d = reinterpret_cast<DwarfInfoPrivate*>(obj);
    return d->elfFile->byteOrder() == ELFDATA2LSB ? DW_END_little : DW_END_big;
}
#else
static Dwarf_Small callback_get_byte_order(void *obj)
{
    const DwarfInfoPrivate *d = reinterpret_cast<DwarfInfoPrivate*>(obj);
    return d->elfFile->byteOrder() == ELFDATA2LSB ? DW_END_little : DW_END_big;
}
#endif

static Dwarf_Small callback_get_length_size(void *obj)
{
    const DwarfInfoPrivate *d = reinterpret_cast<DwarfInfoPrivate*>(obj);
    return d->elfFile->addressSize();
}

static Dwarf_Small callback_get_pointer_size(void *obj)
{
    const DwarfInfoPrivate *d = reinterpret_cast<DwarfInfoPrivate*>(obj);
    return d->elfFile->addressSize();
}

static Dwarf_Unsigned callback_get_section_count(void *obj)
{
    const DwarfInfoPrivate *d = reinterpret_cast<DwarfInfoPrivate*>(obj);
    return d->elfFile->sectionHeaders().size();
}

static void callback_dwarf_handler(Dwarf_Error error, Dwarf_Ptr errarg)
{
    DwarfInfoPrivate *d = reinterpret_cast<DwarfInfoPrivate*>(errarg);

    // Ensure that errcore can be converted
    static_assert( std::is_convertible<Dwarf_Unsigned, unsigned long long>::value, "Incompatible DWARFs" );

    const char *errmsg = dwarf_errmsg(error);
    const Dwarf_Unsigned errcode = dwarf_errno(error);
    if (d->elfFile->isSeparateDebugFile()) {
        qWarning("DWARF error in debug file for %s: %s (errno %llu)",
                 qPrintable(d->elfFile->contentFile()->fileName()), errmsg, static_cast<unsigned long long>(errcode));
    } else {
        qWarning("DWARF error in %s: %s (errno %llu)",
                 qPrintable(d->elfFile->fileName()), errmsg, static_cast<unsigned long long>(errcode));
    }

    d->isValid = false;
}

#ifdef Q_OS_FREEBSD
static int callback_get_section_info(void *obj, Dwarf_Half index, Dwarf_Obj_Access_Section_a *sectionInfo, int *error)
#else
static int callback_get_section_info(void *obj, Dwarf_Half index, Dwarf_Obj_Access_Section *sectionInfo, int *error)
#endif
{
    const DwarfInfoPrivate *d = reinterpret_cast<DwarfInfoPrivate*>(obj);
    const auto sectionHeader = d->elfFile->sectionHeaders().at(index);
#ifdef Q_OS_FREEBSD
    sectionInfo->addr = (Dwarf_Addr)(d->elfFile->rawData() + sectionHeader->sectionOffset());
    sectionInfo->size = sectionHeader->size();
    sectionInfo->name = sectionHeader->name();
#else
    sectionInfo->as_addr = (Dwarf_Addr)(d->elfFile->rawData() + sectionHeader->sectionOffset());
    sectionInfo->as_size = sectionHeader->size();
    sectionInfo->as_name = sectionHeader->name();
#endif
    *error = DW_DLV_OK;
    return DW_DLV_OK;
}

static int callback_load_section(void *obj, Dwarf_Half index, Dwarf_Small **returnData, int *error)
{
    const DwarfInfoPrivate *d = reinterpret_cast<DwarfInfoPrivate*>(obj);
    const auto sectionHeader = d->elfFile->sectionHeaders().at(index);
    *returnData = d->elfFile->rawData() + sectionHeader->sectionOffset();
    *error = DW_DLV_OK;
    return DW_DLV_OK;
}


static Dwarf_Unsigned callback_get_filesize(void *obj)
{
    const DwarfInfoPrivate *d = reinterpret_cast<DwarfInfoPrivate*>(obj);
    return d->elfFile->size();
}

DwarfInfoPrivate::DwarfInfoPrivate(DwarfInfo *qq) :
    q(qq),
    isValid(true)
{
#ifndef Q_OS_FREEBSD
    objAccessIface.ai_object = this;
    objAccessIface.ai_methods = &objAccessMethods;
    objAccessMethods.om_get_byte_order = callback_get_byte_order;
    objAccessMethods.om_get_length_size = callback_get_length_size;
    objAccessMethods.om_get_pointer_size = callback_get_pointer_size;
    objAccessMethods.om_get_section_count = callback_get_section_count;
    objAccessMethods.om_get_section_info = callback_get_section_info;
    objAccessMethods.om_load_section = callback_load_section;
    objAccessMethods.om_get_filesize = callback_get_filesize;

    // TODO: check structure fields at cmake or compile time
    //       rather than ifdeffing on an OS.
    //
    // FreeBSD's DWARF.h doesn't have this (separate developments).
    objAccessMethods.om_relocate_a_section = nullptr;
#else
    objAccessIface.object = this;
    objAccessIface.methods = &objAccessMethods;
    objAccessMethods.get_byte_order = callback_get_byte_order;
    objAccessMethods.get_length_size = callback_get_length_size;
    objAccessMethods.get_pointer_size = callback_get_pointer_size;
    objAccessMethods.get_section_count = callback_get_section_count;
    objAccessMethods.get_section_info = callback_get_section_info;
    objAccessMethods.load_section = callback_load_section;
#endif
}

DwarfInfoPrivate::~DwarfInfoPrivate()
{
    qDeleteAll(compilationUnits);
#ifdef Q_OS_FREEBSD
    dwarf_object_finish(dbg, nullptr);
#else
    dwarf_object_finish(dbg);
#endif
}

void DwarfInfoPrivate::scanCompilationUnits()
{
    Dwarf_Unsigned nextHeader = 0;
    forever {
#ifdef Q_OS_FREEBSD
        int res = dwarf_next_cu_header(dbg, nullptr, nullptr, nullptr, nullptr, &nextHeader, nullptr);
        if (res != DW_DLV_OK)
            return;

        Dwarf_Die cuDie = nullptr;
        res = dwarf_siblingof(dbg, nullptr, &cuDie, nullptr);
#else
        int res = dwarf_next_cu_header_d(dbg,
            true,
            nullptr, nullptr,
            nullptr, nullptr,
            nullptr, nullptr,
            nullptr, nullptr,
            &nextHeader,
            nullptr, nullptr);
        if (res != DW_DLV_OK)
            return;

        Dwarf_Die cuDie = nullptr;
        res = dwarf_siblingof_b(dbg, nullptr, true, &cuDie, nullptr);
#endif
        if(res != DW_DLV_OK)
            return;

        compilationUnits.push_back(new DwarfCuDie(cuDie, q));
    }
}


DwarfDie* DwarfInfoPrivate::dieForMangledSymbolRecursive(const QByteArray& symbol, DwarfDie *die) const
{
    if (die->attribute(DW_AT_linkage_name).toByteArray() == symbol)
        return die;
    foreach (auto childDie, die->children()) {
        const auto hit = dieForMangledSymbolRecursive(symbol, childDie);
        if (hit)
            return hit;
    }
    return nullptr;
}

static int dwarf_object_init(Dwarf_Obj_Access_Interface_a* obj,
    Dwarf_Handler errhand,
    Dwarf_Ptr errarg, Dwarf_Debug* ret_dbg,
    Dwarf_Error* error)
{
    return dwarf_object_init_b(obj,errhand,errarg,
        DW_GROUPNUMBER_ANY,ret_dbg,error);
}

DwarfInfo::DwarfInfo(ElfFile* elfFile) :
    d(new DwarfInfoPrivate(this))
{
    d->elfFile = elfFile;

    Dwarf_Error err  = 0;
    if (dwarf_object_init(&d->objAccessIface, &callback_dwarf_handler, d.get(), &d->dbg, &err) != DW_DLV_OK) {
        qDebug() << "error loading dwarf data";
    }
}

DwarfInfo::~DwarfInfo()
{
    // make sure this is destroyed before d is reset, needs dwarfHandle in its dtor
    delete d->aranges;
}

ElfFile* DwarfInfo::elfFile() const
{
    if (d->elfFile->isSeparateDebugFile())
        return d->elfFile->contentFile();
    return d->elfFile;
}

DwarfAddressRanges* DwarfInfo::addressRanges() const
{
    if (!d->aranges)
        d->aranges = new DwarfAddressRanges(const_cast<DwarfInfo*>(this));
    return d->aranges;
}

Dwarf_Debug DwarfInfo::dwarfHandle() const
{
    return d->dbg;
}

QVector< DwarfCuDie* > DwarfInfo::compilationUnits() const
{
    if (d->compilationUnits.isEmpty())
        d->scanCompilationUnits();
    return d->compilationUnits;
}

DwarfCuDie* DwarfInfo::compilationUnitForAddress(uint64_t address) const
{
    auto cu = addressRanges()->compilationUnitForAddress(address);
    if (cu)
        return cu;

    foreach (auto cu, compilationUnits()) {
        auto ranges = cu->attribute(DW_AT_ranges).value<DwarfRanges>();
        for (int i = 0; i < ranges.size(); ++i) {
            auto range = ranges.entry(i);
            if (range->dwr_type == DW_RANGES_ENTRY && range->dwr_addr1 <= address && address < range->dwr_addr2)
                return cu;
            // TODO DW_RANGES_ADDRESS_SELECTION
        }
    }

    return nullptr;
}

DwarfDie* DwarfInfo::dieAtOffset(Dwarf_Off offset) const
{
    const auto cus = compilationUnits();
    if (cus.isEmpty())
        return nullptr;

    auto it = std::lower_bound(cus.begin(), cus.end(), offset, [](DwarfDie* lhs, Dwarf_Off rhs) { return lhs->offset() < rhs; });

    if (it != cus.end() && (*it)->offset() == offset)
        return *it;

    Q_ASSERT(it != cus.begin());
    --it;
    return (*it)->dieAtOffset(offset);
}

DwarfDie* DwarfInfo::dieForMangledSymbol(const QByteArray& symbol) const
{
    foreach (auto die, compilationUnits()) {
        const auto hit = d->dieForMangledSymbolRecursive(symbol, die);
        if (hit)
            return hit;
    }
    return nullptr;
}

bool DwarfInfo::isValid() const
{
    return d->isValid;
}
