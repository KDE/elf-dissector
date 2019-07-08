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

class DwarfInfoPrivate {
public:
    DwarfInfoPrivate(DwarfInfo* qq);
    ~DwarfInfoPrivate();

    void scanCompilationUnits();
    DwarfDie *dieForMangledSymbolRecursive(const QByteArray &symbol, DwarfDie *die) const;

    ElfFile *elfFile = nullptr;
    QVector<DwarfCuDie*> compilationUnits;
    Dwarf_Obj_Access_Interface objAccessIface;
    Dwarf_Obj_Access_Methods objAccessMethods;

    Dwarf_Debug dbg;

    DwarfInfo *q;
    DwarfAddressRanges *aranges = nullptr;

    bool isValid;
};


static Dwarf_Endianness callback_get_byte_order(void *obj)
{
    const DwarfInfoPrivate *d = reinterpret_cast<DwarfInfoPrivate*>(obj);
    return d->elfFile->byteOrder() == ELFDATA2LSB ? DW_OBJECT_LSB : DW_OBJECT_MSB;
}

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

    const char *errmsg = dwarf_errmsg(error);
    const Dwarf_Unsigned errcode = dwarf_errno(error);
    if (d->elfFile->isSeparateDebugFile()) {
        qWarning("DWARF error in debug file for %s: %s (errno %llu)",
                 qPrintable(d->elfFile->contentFile()->fileName()), errmsg, errcode);
    } else {
        qWarning("DWARF error in %s: %s (errno %llu)",
                 qPrintable(d->elfFile->fileName()), errmsg, errcode);
    }

    d->isValid = false;
}

static int callback_get_section_info(void *obj, Dwarf_Half index, Dwarf_Obj_Access_Section *sectionInfo, int *error)
{
    const DwarfInfoPrivate *d = reinterpret_cast<DwarfInfoPrivate*>(obj);
    const auto sectionHeader = d->elfFile->sectionHeaders().at(index);
    sectionInfo->addr = (Dwarf_Addr)(d->elfFile->rawData() + sectionHeader->sectionOffset());
    sectionInfo->size = sectionHeader->size();
    sectionInfo->name = sectionHeader->name();
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


DwarfInfoPrivate::DwarfInfoPrivate(DwarfInfo *qq) :
    q(qq),
    isValid(true)
{
    objAccessIface.object = this;
    objAccessIface.methods = &objAccessMethods;
    objAccessMethods.get_byte_order = callback_get_byte_order;
    objAccessMethods.get_length_size = callback_get_length_size;
    objAccessMethods.get_pointer_size = callback_get_pointer_size;
    objAccessMethods.get_section_count = callback_get_section_count;
    objAccessMethods.get_section_info = callback_get_section_info;
    objAccessMethods.load_section = callback_load_section;
    objAccessMethods.relocate_a_section = nullptr;
}

DwarfInfoPrivate::~DwarfInfoPrivate()
{
    qDeleteAll(compilationUnits);
    dwarf_object_finish(dbg, nullptr);
}

void DwarfInfoPrivate::scanCompilationUnits()
{
    Dwarf_Unsigned nextHeader = 0;
    forever {
        auto res = dwarf_next_cu_header(dbg, nullptr, nullptr, nullptr, nullptr, &nextHeader, nullptr);
        if (res != DW_DLV_OK)
            return;

        Dwarf_Die cuDie = nullptr;
        res = dwarf_siblingof(dbg, nullptr, &cuDie, nullptr);
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

DwarfInfo::DwarfInfo(ElfFile* elfFile) :
    d(new DwarfInfoPrivate(this))
{
    d->elfFile = elfFile;

    if (dwarf_object_init(&d->objAccessIface, &callback_dwarf_handler, d.get(), &d->dbg, nullptr) != DW_DLV_OK) {
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
