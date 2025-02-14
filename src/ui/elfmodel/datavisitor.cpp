/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "datavisitor.h"
#include "elfmodel.h"

#include <elf/elffile.h>
#include <elf/elfnoteentry.h>
#include <elf/elfgnusymbolversiontable.h>
#include <elf/elfgnusymbolversiondefinitionssection.h>
#include <elf/elfgnusymbolversiondefinitionauxiliaryentry.h>
#include <elf/elfgnusymbolversionrequirement.h>
#include <elf/elfgnusymbolversionrequirementauxiliaryentry.h>
#include <elf/elfsegmentheader.h>
#include <elf.h>

#if HAVE_DWARF
#include <dwarf/dwarfaddressranges.h>
#endif

#include <disassmbler/disassembler.h>
#include <demangle/demangler.h>
#include <checks/structurepackingcheck.h>
#include <navigator/codenavigatorprinter.h>

#include <printers/dwarfprinter.h>
#include <printers/dynamicsectionprinter.h>
#include <printers/elfprinter.h>
#include <printers/gnuversionprinter.h>
#include <printers/notesectionprinter.h>
#include <printers/relocationprinter.h>
#include <printers/symbolprinter.h>

#include <QIcon>
#include <QUrl>

#include <cassert>

using namespace Qt::Literals;

class NavigatingDisassembler : public Disassembler
{
public:
    NavigatingDisassembler(const DataVisitor *v) : m_v(v) {}

    QString printSymbol(ElfSymbolTableEntry* entry) const override
    {
        return m_v->printSymbolName(entry);
    }

    QString printGotEntry(ElfGotEntry* entry) const override
    {
        QString s;
        s += QLatin1String("<a href=\"");
        s += QString::fromUtf8(m_v->m_model->indexForNode(entry).data(ElfModel::NodeUrl).toUrl().toEncoded());
        s += QLatin1String("\">");
        s += Disassembler::printGotEntry(entry);
        s += QLatin1String("</a>");
        return s;
    }

    QString printPltEntry(ElfPltEntry* entry) const override
    {
        QString s;
        s += QLatin1String("<a href=\"");
        s += QString::fromUtf8(m_v->m_model->indexForNode(entry).data(ElfModel::NodeUrl).toUrl().toEncoded());
        s += QLatin1String("\">");
        s += Disassembler::printPltEntry(entry);
        s += QLatin1String("</a>");
        return s;
    }

private:
    const DataVisitor* const m_v;
};

DataVisitor::DataVisitor(const ElfModel* model, int column) :
    m_model(model),
    m_column(column)
{
}

QVariant DataVisitor::doVisit(ElfFile* file, int arg) const
{
    switch (arg) {
        case Qt::DisplayRole:
            return file->displayName();
        case Qt::ToolTipRole:
            return file->fileName();
        case ElfModel::SizeRole:
            return QVariant::fromValue<uint64_t>(file->size());
        case ElfModel::DetailRole:
        {
            QString s;
            s += "File name: "_L1 + file->fileName() + "<br/>"_L1;
            s += QLatin1String("Address size: ") + (file->type() == ELFCLASS32 ? "32 bit"_L1 : "64 bit"_L1) + "<br/>"_L1;
            s += QLatin1String("Byte order: ") + (file->byteOrder() == ELFDATA2LSB ? "little endian"_L1 : "big endian"_L1) + "<br/>"_L1;
            s += QLatin1String("File type: ") + QString::fromUtf8(ElfPrinter::fileType(file->header()->type())) + "<br/>"_L1;
            s += QLatin1String("Machine: ") + QString::fromUtf8(ElfPrinter::machine(file->header()->machine())) + "<br/>"_L1;
            s += QLatin1String("OS ABI: ") + QString::fromUtf8(ElfPrinter::osAbi(file->osAbi())) + "<br/>"_L1;
            s += "Flags: "_L1 + QString::number(file->header()->flags()) + "<br/>"_L1;
            s += QLatin1String("Entry point: 0x") + QString::number(file->header()->entryPoint(), 16);
            if (file->header()->entryPoint() && file->symbolTable()) {
                const auto entry = file->symbolTable()->entryWithValue(file->header()->entryPoint());
                if (entry)
                    s += " ("_L1 + printSymbolName(entry) + ')'_L1;
            }
            if (file->separateDebugFile())
                s += "<br/>Separate debug file: "_L1 + file->separateDebugFile()->fileName();
            s += QLatin1String("<br/><br/>");

            s += QLatin1String("<b>Segments</b></br>");
            s += QLatin1String("<table border=\"1\" border-style=\"solid\" cellspacing=\"0\">");
            s += QLatin1String("<tr><th>Type</th><th>Flags</th><th>Offset</th><th>Virtual Address</th>");
            s += QLatin1String("<th>Physical Address</th><th>File Size</th><th>Memory Size</th><th>Alignment</th></tr>");
            for  (auto phdr : file->segmentHeaders()) {
                s += QLatin1String("<tr>");
                s += "<td>"_L1 + QString::fromUtf8(ElfPrinter::segmentType(phdr->type())) + "</td>"_L1;
                s += "<td>"_L1 + QString::fromUtf8(ElfPrinter::segmentFlags(phdr->flags())) + "</td>"_L1;
                s += "<td>0x"_L1 + QString::number(phdr->offset(), 16) + "</td>"_L1;
                s += "<td>0x"_L1 + QString::number(phdr->virtualAddress(), 16) + "</td>"_L1;
                s += "<td>0x"_L1 + QString::number(phdr->physicalAddress(), 16) + "</td>"_L1;
                s += "<td>"_L1 + QString::number(phdr->fileSize()) + "</td>"_L1;
                s += "<td>"_L1 + QString::number(phdr->memorySize()) + "</td>"_L1;
                s += "<td>0x"_L1 + QString::number(phdr->alignment(), 16) + "</td>"_L1;
                s += QLatin1String("</tr>");
            }
            s += QLatin1String("</table");
            return s;
        }
        case ElfModel::FileRole:
            return QVariant::fromValue(file);
    }
    return QVariant();
}

QVariant DataVisitor::doVisit(ElfSection* section, int arg) const
{
    switch (arg) {
        case Qt::DisplayRole:
            if (*section->header()->name() == 0)
                return QObject::tr("<null>");
            return QString::fromUtf8(section->header()->name());
        case Qt::DecorationRole:
            if (m_column == 0 && section->file()->isSeparateDebugFile())
                return QIcon::fromTheme(QStringLiteral("package_development_debugger"));
            return {};
        case ElfModel::SizeRole:
            return QVariant::fromValue<uint64_t>(section->size());
        case ElfModel::DetailRole:
        {
            QString s(QStringLiteral("<b>Section</b><br/>"));
            s += QLatin1String("Name: ") + QString::fromUtf8(section->header()->name()) + "<br/>"_L1;
            s += QLatin1String("Size: ") + QString::number(section->header()->size()) + " bytes<br/>"_L1;
            s += QLatin1String("Flags: ") + QString::fromUtf8(ElfPrinter::sectionFlags(section->header()->flags())) + "<br/>"_L1;
            s += QLatin1String("Offset: 0x") + QString::number(section->header()->sectionOffset(), 16) + "<br/>"_L1;
            s += QLatin1String("Virtual Address: 0x") + QString::number(section->header()->virtualAddress(), 16) + "<br/>"_L1;
            s += QLatin1String("Type: ") + QString::fromUtf8(ElfPrinter::sectionType(section->header()->type())) + "<br/>"_L1;
            if (section->header()->link())
                s += "Linked section: "_L1 + printSectionName(section->linkedSection<ElfSection>()) + "<br/>"_L1;
            if (section->header()->flags() & SHF_INFO_LINK)
                s += "Referenced section: "_L1 + printSectionName(section->file()->section<ElfSection>(section->header()->info())) + "<br/>"_L1;
            if (section->header()->entrySize()) {
                s += QLatin1String("Entries: ") + QString::number(section->header()->entryCount())
                  + " x "_L1 + QString::number(section->header()->entrySize()) + " byte<br/>"_L1;
            }
            if (section->header()->flags() & SHF_STRINGS && section->size() < 100000) { // QTextBrowser fails on too large input
                s += QLatin1String("Strings:<br/>");
                uint prevIndex = 0, index = 0;
                do {
                    if (section->rawData()[index] == 0) {
                        if (index - prevIndex > 0) {
                            const auto b = QByteArray::fromRawData(reinterpret_cast<const char*>(section->rawData() + prevIndex), index - prevIndex);
                            s += QString::number(prevIndex) + ": "_L1 + QString::fromUtf8(b) + "<br/>"_L1;
                        }
                        prevIndex = index + 1;
                    }
                    ++index;
                } while (index < section->size());
            }
            if (strcmp(section->header()->name(), ".init") == 0 || strcmp(section->header()->name(), ".fini") == 0) {
                NavigatingDisassembler da(this);
                s += "Code:<br/><tt>"_L1 + da.disassemble(section) + "</tt>"_L1;
            }
            if (section->header()->type() == SHT_INIT_ARRAY || section->header()->type() == SHT_FINI_ARRAY) {
                const auto addrSize = section->file()->addressSize();
                s += QLatin1String("Content:<br/>");
                for (uint i = 0; i < section->header()->size() / addrSize; ++i) {
                    uint64_t value = 0;
                    memcpy(&value, section->rawData() + i * addrSize, addrSize);
                    s += QString::number(i) + ": 0x"_L1 + QString::number(value, 16);
                    if (value) {
                        const auto ref = section->file()->symbolTable()->entryWithValue(value);
                        if (ref)
                            s += QLatin1Char(' ') + printSymbolName(ref);
                    } else { // check for relocation
                        const auto relocEntry = section->file()->reverseRelocator()->find(section->header()->virtualAddress() + i * addrSize);
                        if (relocEntry) {
                            const auto sym = section->file()->symbolTable()->entryContainingValue(relocEntry->relocationTarget());
                            if (sym)
                                s += " relocated to: "_L1 + printSymbolName(sym);
                        }
                    }

                    s += QLatin1String("<br/>");
                }
            }
            return s;
        }
        case ElfModel::SectionRole:
            return QVariant::fromValue(section);
    }

    return QVariant();
}

QVariant DataVisitor::doVisit(ElfSymbolTableSection* symtab, int arg) const
{
    const auto base = doVisit(static_cast<ElfSection*>(symtab), arg);
    if (arg != ElfModel::DetailRole)
        return base;
    QString s = base.toString();
    s += QLatin1String("<br/><b>Symbol Table</b>");
    s += "<br/>Exported symbols: "_L1 + QString::number(symtab->exportCount());
    s += "<br/>Imported symbols: "_L1 + QString::number(symtab->importCount());
    s += QLatin1String("<br/>");
    return s;
}

static uint64_t virtualTableEntry(ElfSymbolTableEntry *entry, int index)
{
    const auto addrSize = entry->symbolTable()->file()->addressSize();
    assert(entry->size() % addrSize == 0);

    // TODO this assumes endianess equalness
    if (addrSize == 8)
        return *reinterpret_cast<const uint64_t*>(entry->data() + index * addrSize);
    if (addrSize == 4)
        return *reinterpret_cast<const uint32_t*>(entry->data() + index * addrSize);

    Q_UNREACHABLE();
}

static QString printVerSymInfo(ElfSymbolTableEntry *entry)
{
    if (entry->symbolTable()->header()->type() != SHT_DYNSYM)
        return {};
    const ElfFile *file = entry->symbolTable()->file();
    const auto verSymIdx = file->indexOfSection(SHT_GNU_versym);
    if (verSymIdx < 0)
        return {};
    const auto verSymTab = file->section<ElfGNUSymbolVersionTable>(verSymIdx);
    const auto versionIndex = verSymTab->versionIndex(entry->index());

    QString s(QStringLiteral("GNU version: "));
    switch (versionIndex) {
        case VER_NDX_LOCAL:
            s += QLatin1String("&lt;local&gt;");
            break;
        case VER_NDX_GLOBAL:
            s += QLatin1String("&lt;global&gt;");
            break;
        default:
        {
            auto f = entry->symbolTable()->file();
            if (entry->hasValidSection()) { // definition
                const auto verDefIdx = f->indexOfSection(SHT_GNU_verdef);
                if (verDefIdx <= 0) {
                    s += QString::number(versionIndex);
                } else {
                    auto verDefSection = f->section<ElfGNUSymbolVersionDefinitionsSection>(verDefIdx);
                    assert(verDefSection);
                    s += QString::fromUtf8(verDefSection->definitionForVersionIndex(versionIndex)->auxiliaryEntry(0)->name());
                }
                break;
            } else { // requirement
                const auto verReqIdx = f->indexOfSection(SHT_GNU_verneed);
                auto verReqSection = f->section<ElfGNUSymbolVersionRequirementsSection>(verReqIdx);
                assert(verReqSection);
                s += QString::fromUtf8(verReqSection->requirementForVersionIndex(versionIndex)->name());
                break;
            }
            s += QString::number(versionIndex);
        }
    };
    if (verSymTab->isHidden(entry->index()))
        s += QLatin1String(" (hidden)");
    s += QLatin1String("<br/>");
    return s;
}

#if HAVE_DWARF
static DwarfDie* findDwarfDie(ElfSymbolTableEntry *entry)
{
    auto *dwarf = entry->symbolTable()->file()->dwarfInfo();
    if (!dwarf || entry->value() == 0)
        return nullptr;

    DwarfDie* res = nullptr;
    if (dwarf->addressRanges()->isValid())
       res = dwarf->addressRanges()->dieForAddress(entry->value());
    if (!res)
        res = dwarf->dieForMangledSymbol(entry->name());
    return res;
}
#endif

QVariant DataVisitor::doVisit(ElfSymbolTableEntry* entry, int arg) const
{
    switch (arg) {
        case Qt::DisplayRole:
            if (*entry->name() == 0)
                return QObject::tr("<null>");
            return QString::fromUtf8(entry->name());
        case ElfModel::SizeRole:
            return QVariant::fromValue<uint64_t>(entry->size());
        case ElfModel::DetailRole:
        {
            QString s(QStringLiteral("<b>Symbol</b><br/>"));
            s += QLatin1String("Mangled name: ") + QString::fromUtf8(entry->name()) + "<br/>"_L1;
            Demangler demangler;
            s += QLatin1String("Demangled name: ") + QString::fromUtf8(demangler.demangleFull(entry->name())).toHtmlEscaped() + "<br/>"_L1;
            s += QLatin1String("Size: ") + QString::number(entry->size()) + "<br/>"_L1;
            s += QLatin1String("Value: 0x") + QString::number(entry->value(), 16) + "<br/>"_L1;
            s += QLatin1String("Bind type: ") + QString::fromUtf8(SymbolPrinter::bindType(entry->bindType())) + "<br/>"_L1;
            s += QLatin1String("Symbol type: ") + QString::fromUtf8(SymbolPrinter::symbolType(entry->type())) + "<br/>"_L1;
            s += QLatin1String("Visibility: ") + QString::fromUtf8(SymbolPrinter::visibility(entry->visibility())) + "<br/>"_L1;
            s += printVerSymInfo(entry);

            const auto hasValidSectionIndex = entry->hasValidSection();
            if (hasValidSectionIndex) {
                s += "Section: "_L1 + printSectionName(entry->section()) + "<br/>"_L1;
            }

            if (hasValidSectionIndex && entry->sectionHeader()->type() == SHT_NOBITS) {
                // .bss, i.e. no content to display
            } else if (entry->type() == STT_FUNC && entry->size() > 0) {
                NavigatingDisassembler da(this);
                s += QLatin1String("Code:<br/><tt>") + da.disassemble(entry) + "</tt>"_L1;
            } else if (entry->type() == STT_OBJECT && entry->size() > 0) {
                const auto addrSize = entry->symbolTable()->file()->addressSize();
                const auto symbolType = Demangler::symbolType(entry->name());
                switch (symbolType) {
                    case Demangler::SymbolType::VTable:
                    case Demangler::SymbolType::ConstructionVTable:
                    {
                        s += symbolType == Demangler::SymbolType::ConstructionVTable ? QObject::tr("Construction VTable") : QObject::tr("VTable");
                        s += QLatin1String(":<br/><tt>");
                        for (uint i = 0; i < entry->size() / addrSize; ++i) {
                            const uint64_t v = virtualTableEntry(entry, i);
                            s += QString::number(i) + ": 0x"_L1 + QString::number(v, 16);
                            const auto ref = entry->symbolTable()->entryWithValue(v);
                            if (ref) {
                                s += QLatin1Char(' ') + printSymbolName(ref) + QLatin1String(" (") + QString::fromUtf8(Demangler::demangleFull(ref->name())) + QLatin1Char(')');
                            } else {
                                auto reloc = entry->symbolTable()->file()->reverseRelocator()->find(entry->value() + i * addrSize);
                                if (reloc) {
                                    const auto relocSym = reloc->symbol();
                                    if (relocSym)
                                        s += QLatin1String(" relocation: ") + printSymbolName(relocSym);
                                }
                            }
                            s += QLatin1String("<br/>");
                        }
                        s += QLatin1String("</tt><br/>");
                        break;
                    }
                    case Demangler::SymbolType::VTT:
                    {
                        s += QLatin1String("VTT:<br/><tt>");
                        for (uint i = 0; i < entry->size() / addrSize; ++i) {
                            const uint64_t v = virtualTableEntry(entry, i);
                            s += QString::number(i) + ": 0x"_L1 + QString::number(v, 16);
                            // vptrs point to one after the RTTI entry, which is the first virtual method, unless there is none in that
                            // case we would point past the vtable here, and thus we wont find it, so better look for the RTTI spot.
                            const auto ref = entry->symbolTable()->entryContainingValue(v - addrSize);
                            if (ref) {
                                const auto offset = v - ref->value();
                                s += QLatin1String(" entry ") + QString::number(offset / addrSize) + QLatin1String(" in ") + printSymbolName(ref);
                                s += QLatin1String(" (") + QString::fromUtf8(Demangler::demangleFull(ref->name())) + QLatin1Char(')');
                            }
                            s += QLatin1String("<br/>");
                        }
                        s += QLatin1String("</tt><br/>");
                        break;
                    }
                    case Demangler::SymbolType::TypeInfoName:
                        s += "Type info name: "_L1 + QString::fromUtf8(QByteArray((const char*)entry->data())) + "<br/>"_L1;
                        break;
                    // TODO: add other symbol types
                    default:
                        s += QStringLiteral("Data:<br/><tt>");
                        const unsigned char* data = entry->data();
                        s += QString::fromLatin1(QByteArray::fromRawData((const char*)data, entry->size()).toHex());
                        s += QLatin1String("</tt><br/>");
                }
            }

#if HAVE_DWARF
            const auto die = findDwarfDie(entry);
            if (die) {
                s += CodeNavigatorPrinter::sourceLocationRichText(die);
                s += QLatin1String("<br/><b>DWARF DIE</b><br/>");
                s += printDwarfDie(die);
            }
#endif
            return s;
        }
    }

    return QVariant();
}

QVariant DataVisitor::doVisit(ElfGnuDebugLinkSection* section, int role) const
{
    auto baseData = doVisit(static_cast<ElfSection*>(section), role);
    if (role == ElfModel::DetailRole) {
        QString s = baseData.toString();
        s += QLatin1String("<br/><b>Debug Link</b>");
        s += "<br/>File name: "_L1 + QString::fromUtf8(section->fileName());
        s += "<br/>CRC: 0x"_L1 + QString::number(section->crc(), 16);
        return s;
    }
    return baseData;
}

QVariant DataVisitor::doVisit(ElfDynamicEntry *entry, int arg) const
{
    switch (arg) {
        case Qt::DisplayRole:
            return entry->tagName();
        case ElfModel::DetailRole:
        {
            QString s;
            s += QLatin1String("Tag name: ") + entry->tagName() + "<br/>"_L1;
            s += QStringLiteral("Value: ");
            switch (entry->tag()) {
                case DT_FLAGS:
                    s += QString::fromUtf8(DynamicSectionPrinter::flagsToDescriptions(entry->value()));
                    break;
                case DT_FLAGS_1:
                    s += QString::fromUtf8(DynamicSectionPrinter::flags1ToDescriptions(entry->value()));
                    break;
                case DT_PLTREL:
                    s += QString::fromUtf8(RelocationPrinter::label(entry->dynamicSection()->file()->header()->machine(), entry->value()));
                    break;
                default:
                    if (entry->isStringValue()) {
                        s+= QString::fromUtf8(entry->stringValue());
                    } else if (entry->isAddress()) {
                        s += QLatin1String("0x") + QString::number(entry->pointer(), 16);
                        const auto secIdx = entry->dynamicSection()->file()->indexOfSectionWithVirtualAddress(entry->pointer());
                        if (secIdx >= 0) {
                            const auto section = entry->dynamicSection()->file()->section<ElfSection>(secIdx);
                            assert(section);
                            s += " ("_L1 + printSectionName(section);
                            if (section->header()->virtualAddress() < entry->pointer())
                                s += " + 0x"_L1 + QString::number(entry->pointer() - section->header()->virtualAddress(), 16);
                            s += ')'_L1;
                        }
                    } else {
                        s += QString::number(entry->value());
                    }
            }
            s += QLatin1String("<br/>");
            return s;
        }
    }

    return QVariant();
}

QVariant DataVisitor::doVisit(ElfHashSection* section, int role) const
{
    const auto base = doVisit(static_cast<ElfSection*>(section), role);
    if (role == ElfModel::DetailRole) {
        QString s = base.toString();
        s += QLatin1String("<br/><b>Hash Table</b><br/>");
        s += "Buckets: "_L1 + QString::number(section->bucketCount()) + "<br/>"_L1;
        s += "Chains: "_L1 + QString::number(section->chainCount()) + "<br/>"_L1;
        if (ElfGnuHashSection *gnuHash = dynamic_cast<ElfGnuHashSection*>(section)) {
            s += "Symbol table index: "_L1 + QString::number(gnuHash->symbolIndex()) + "<br/>"_L1;
            s += "Mask word count: "_L1 + QString::number(gnuHash->maskWordsCount()) + "<br/>"_L1;
            s += "Shift count: "_L1 + QString::number(gnuHash->shift2()) + "<br/>"_L1;
        }

        s += QLatin1String("Chain length histogram<br/>");
        const auto hist = section->histogram();
        for (int i = 0; i < hist.size(); ++i) {
            s += "&nbsp;&nbsp;"_L1 + QString::number(i) + ": "_L1 + QString::number(hist.at(i)) + "<br/>"_L1;
        }
        s += "Average collision common prefix length: "_L1 + QString::number(section->averagePrefixLength()) + "<br/>"_L1;
        return s;
    }
    return base;
}

QVariant DataVisitor::doVisit(ElfGNUSymbolVersionDefinition* verDef, int role) const
{
    switch (role) {
        case Qt::DisplayRole:
            return u"version definition"_s;
        case ElfModel::SizeRole:
            return verDef->size();
        case ElfModel::DetailRole:
        {
            QString s;
            s += "Flags: "_L1 + QString::fromUtf8(GnuVersionPrinter::versionFlags(verDef->flags())) + "<br/>"_L1;
            s += "Index: "_L1 + QString::number(verDef->versionIndex()) + "<br/>"_L1;
            s += "Aux count: "_L1 + QString::number(verDef->auxiliarySize()) + "<br/>"_L1;
            s += "Hash: "_L1 + QString::number(verDef->hash()) + "<br/>"_L1;
            s += "Aux offset: "_L1 + QString::number(verDef->auxiliaryOffset()) + "<br/>"_L1;
            s += "Next offset: "_L1 + QString::number(verDef->nextOffset()) + "<br/>"_L1;
            return s;
        }
    }
    return {};
}

QVariant DataVisitor::doVisit(ElfGNUSymbolVersionDefinitionAuxiliaryEntry* auxEntry, int role) const
{
    switch (role) {
        case Qt::DisplayRole:
            return QString::fromUtf8(auxEntry->name());
        case ElfModel::SizeRole:
            return (int)sizeof(Elf64_Verdaux);
        case ElfModel::DetailRole:
        {
            QString s;
            s += QLatin1String("Name: ") + QString::fromUtf8(auxEntry->name()) + "<br/>"_L1;
            s += "Next: "_L1 + QString::number(auxEntry->nextAuxiliaryEntryOffset()) + "<br/>"_L1;
            return s;
        }
    }
    return {};
}

QVariant DataVisitor::doVisit(ElfGNUSymbolVersionRequirement *verNeed, int role) const
{
    switch (role) {
        case Qt::DisplayRole:
            return QString::fromUtf8(verNeed->fileName());
        case ElfModel::SizeRole:
            return verNeed->size();
        case ElfModel::DetailRole:
        {
            QString s;
            s += "File: "_L1 + QString::fromUtf8(verNeed->fileName()) + "<br/>"_L1;
            s += "Aux count: "_L1 + QString::number(verNeed->auxiliarySize()) + "<br/>"_L1;
            s += "Aux offset: "_L1 + QString::number(verNeed->auxiliaryOffset()) + "<br/>"_L1;
            s += "Next offset: "_L1 + QString::number(verNeed->nextOffset()) + "<br/>"_L1;
            return s;
        }
    }
    return {};
}

QVariant DataVisitor::doVisit(ElfGNUSymbolVersionRequirementAuxiliaryEntry* auxEntry, int role) const
{
    switch (role) {
        case Qt::DisplayRole:
            return QString::fromUtf8(auxEntry->name());
        case ElfModel::SizeRole:
            return (int)sizeof(Elf64_Vernaux);
        case ElfModel::DetailRole:
        {
            QString s;
            s += QLatin1String("Name: ") + QString::fromUtf8(auxEntry->name()) + "<br/>"_L1;
            s += "Hash: "_L1 + QString::number(auxEntry->hash()) + "<br/>"_L1;
            s += "Flags: "_L1 + QString::number(auxEntry->flags()) + "<br/>"_L1;
            s += "Other: "_L1 + QString::number(auxEntry->other()) + "<br/>"_L1;
            s += "Next: "_L1 + QString::number(auxEntry->nextAuxiliaryEntryOffset()) + "<br/>"_L1;
            return s;
        }
    }
    return {};
}

QVariant DataVisitor::doVisit(ElfGotEntry* entry, int role) const
{
    switch (role) {
        case Qt::DisplayRole:
        {
            const auto reloc = entry->relocation();
            const auto sym = reloc ? reloc->symbol() : nullptr;
            if (sym)
                return QString(QString::fromUtf8(sym->name()) + QStringLiteral("@got"));
            return QString("GOT entry "_L1 + QString::number(entry->index()));
        }
        case ElfModel::SizeRole:
            return entry->section()->file()->addressSize();
        case ElfModel::DetailRole:
        {
            QString s;
            s += QLatin1String("<b>GOT entry</b><br/>");
            s += "Address: 0x"_L1 + QString::number(entry->address(), 16) + "<br/><br/>"_L1;
            const auto reloc = entry->relocation();
            s += printRelocation(reloc);
            return s;
        }
    }
    return {};
}

QVariant DataVisitor::doVisit(ElfNoteEntry* entry, int role) const
{
    switch (role) {
        case Qt::DisplayRole:
            return QString::fromUtf8(NoteSectionPrinter::typeDisplayString(entry));
        case ElfModel::SizeRole:
            return QVariant::fromValue<quint64>(entry->size());
        case ElfModel::DetailRole:
        {
            QString s(QStringLiteral("Name: "));
            s += QString::fromUtf8(entry->name()) + QStringLiteral("<br/>Description: ");
            if (entry->isGNUVendorNote() && entry->type() == NT_GNU_ABI_TAG) {
                s += QString::fromUtf8(NoteSectionPrinter::abi(entry));
            } else if (entry->isGNUVendorNote() && entry->type() == NT_GNU_GOLD_VERSION) {
                s += QString::fromUtf8(QByteArrayView(entry->descriptionData(), entry->descriptionSize()));
            } else {
                s += QString::fromUtf8(QByteArray(entry->descriptionData(), entry->descriptionSize()).toHex());
            }
            return s;
        }
    }

    return QVariant();
}

QVariant DataVisitor::doVisit(ElfPltEntry* entry, int role) const
{
    switch (role) {
        case Qt::DisplayRole:
        {
            const auto gotEntry = entry->gotEntry();
            const auto reloc = gotEntry ? gotEntry->relocation() : nullptr;
            const auto sym = reloc ? reloc->symbol() : nullptr;
            if (sym)
                return QString(QString::fromUtf8(sym->name()) + QStringLiteral("@plt"));
            return QString("PLT entry "_L1 + QString::number(entry->index()));
        }
        case ElfModel::SizeRole:
            return QVariant::fromValue<quint64>(entry->size());
        case ElfModel::DetailRole:
        {
            QString s(QStringLiteral("<b>PLT Entry</b><br/>Virtual address: 0x"));
            s += QString::number(entry->section()->header()->virtualAddress() + entry->index() * entry->size(), 16);
            s += QLatin1String("<br/>Code:<br/><tt>");
            NavigatingDisassembler da(this);
            s += da.disassemble(entry);
            s += QLatin1String("</tt><br/>");

            const auto got = entry->gotEntry();
            if (got) {
                // TODO add navigation link
                s += QLatin1String("<b>Corresponding GOT entry</b><br/>");
                s += "Address: 0x"_L1 + QString::number(got->address(), 16) + "<br/><br/>"_L1;
                const auto reloc = got->relocation();
                s += printRelocation(reloc);
            }

            return s;
        }
    }

    return {};
}

QVariant DataVisitor::doVisit(ElfRelocationEntry *entry, int arg) const
{
    switch (arg) {
        case Qt::DisplayRole:
            return QString(QString::fromUtf8(RelocationPrinter::label(entry)) + " 0x"_L1 + QString::number(entry->offset(), 16));
            break;
        case ElfModel::DetailRole:
            return printRelocation(entry);
    }

    return {};
}

#if HAVE_DWARF
QVariant DataVisitor::doVisit(DwarfInfo* info, int arg) const
{
    const auto sectionIndex = info->elfFile()->indexOfSection(".debug_info");
    const auto section = info->elfFile()->section<ElfSection>(sectionIndex);
    return doVisit(section, arg);
}

QVariant DataVisitor::doVisit(DwarfDie* die, int arg) const
{
    switch (arg) {
        case Qt::DisplayRole:
            return die->displayName();
        case ElfModel::DetailRole:
        {
            QString s = printDwarfDie(die);
            s += CodeNavigatorPrinter::sourceLocationRichText(die);

            if ((die->tag() == DW_TAG_structure_type || die->tag() == DW_TAG_class_type) && die->typeSize() > 0) {
                s += QLatin1String("<tt><pre>");
                StructurePackingCheck check;
                check.setElfFileSet(m_model->fileSet());
                s += check.checkOneStructure(die).toHtmlEscaped();
                s += QLatin1String("</pre></tt><br/>");
            }

            return s;
        }
    }
    return {};
}
#endif

QString DataVisitor::printSectionName(ElfSection* section) const
{
    const auto idx = m_model->indexForNode(section);
    const auto url = idx.data(ElfModel::NodeUrl).toUrl();

    QString s(QStringLiteral("<a href=\""));
    s += QString::fromUtf8(url.toEncoded());
    s += QLatin1String("\">");
    s += QString::fromUtf8(section->header()->name());
    s += QLatin1String("</a>");

    return s;
}

QString DataVisitor::printSymbolName(ElfSymbolTableEntry* symbol) const
{
    const auto idx = m_model->indexForNode(symbol);
    const auto url = idx.data(ElfModel::NodeUrl).toUrl();

    QString s(QStringLiteral("<a href=\""));
    s += QString::fromUtf8(url.toEncoded());
    s += QLatin1String("\">");
    s += QString::fromUtf8(symbol->name());
    s += QLatin1String("</a>");

    return s;
}

QString DataVisitor::printRelocation(ElfRelocationEntry* entry) const
{
    QString s;
    if (!entry)
        return s;

    s += QLatin1String("<b>Relocation</b><br/>");
    s += "Offset: 0x"_L1 + QString::number(entry->offset(), 16);
    const auto sym = entry->relocationTable()->file()->symbolTable()->entryContainingValue(entry->offset());
    if (sym) {
        s += QLatin1String(" (") + printSymbolName(sym) + " + 0x"_L1 + QString::number(entry->offset() - sym->value(), 16) + ')'_L1;
    } else {
        for (const auto shdr : entry->relocationTable()->file()->sectionHeaders()) {
            if (shdr->virtualAddress() <= entry->offset() && entry->offset() < shdr->virtualAddress() + shdr->size()) {
                s += QLatin1String(" (") + QString::fromUtf8(shdr->name()) + " + 0x"_L1 + QString::number(entry->offset() - shdr->virtualAddress(), 16) + ')'_L1;
                break;
            }
        }
    }
    s += QLatin1String("<br/>");
    s += "Type: "_L1 + QString::fromUtf8(RelocationPrinter::description(entry)) + " ("_L1 + QString::fromUtf8(RelocationPrinter::label(entry)) + ")<br/>"_L1;
    const auto sourceSym = entry->symbol();
    if (sourceSym) {
        s += QLatin1String("Symbol: ") + printSymbolName(sourceSym) + "<br/>"_L1;
        s += QLatin1String("Demangled symbol: ") + QString::fromUtf8(Demangler::demangleFull(sourceSym->name())).toHtmlEscaped() + "<br/>"_L1;
    } else {
        s += QStringLiteral("Symbol: &lt;none&gt;<br/>");
    }
    s += "Addend: 0x"_L1 + QString::number(entry->addend(), 16);
    return s;
}

#if HAVE_DWARF
QString DataVisitor::printDwarfDie(DwarfDie* die) const
{
    assert(die);
    QString s;
    s += "TAG: "_L1 + QLatin1String(die->tagName()) + "<br/>"_L1;
    s += "Name: "_L1 + QString::fromUtf8(die->name()).toHtmlEscaped() + "<br/>"_L1;
    s += "Offset: "_L1 + QString::number(die->offset()) + "<br/>"_L1;
    for (const auto attrType : die->attributes()) {
        const QVariant attrValue = die->attribute(attrType);
        QString attrValueStr;
        if (DwarfDie *die = attrValue.value<DwarfDie*>())
            attrValueStr = printDwarfDieName(die);
        else
            attrValueStr = attrValue.toString().toHtmlEscaped();
        s += QLatin1String(die->attributeName(attrType)) + ": "_L1 + attrValueStr + "<br/>"_L1;
    }
    return s;
}

QString DataVisitor::printDwarfDieName(DwarfDie* die) const
{
    const auto idx = m_model->indexForNode(die);
    const auto url = idx.data(ElfModel::NodeUrl).toUrl();

    QString s(QStringLiteral("<a href=\""));
    s += QString::fromUtf8(url.toEncoded());
    s += QLatin1String("\">");
    s += die->displayName().toHtmlEscaped();
    s += QLatin1String("</a>");

    return s;
}
#endif
