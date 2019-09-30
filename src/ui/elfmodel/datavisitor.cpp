/*
    Copyright (C) 2013-2014 Volker Krause <vkrause@kde.org>

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

#include <dwarf/dwarfaddressranges.h>

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
        s += m_v->m_model->indexForNode(entry).data(ElfModel::NodeUrl).toUrl().toEncoded();
        s += QLatin1String("\">");
        s += Disassembler::printGotEntry(entry);
        s += QLatin1String("</a>");
        return s;
    }

    QString printPltEntry(ElfPltEntry* entry) const override
    {
        QString s;
        s += QLatin1String("<a href=\"");
        s += m_v->m_model->indexForNode(entry).data(ElfModel::NodeUrl).toUrl().toEncoded();
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
            s += "File name: " + file->fileName() + "<br/>";
            s += QLatin1String("Address size: ") + (file->type() == ELFCLASS32 ? "32 bit" : "64 bit") + "<br/>";
            s += QLatin1String("Byte order: ") + (file->byteOrder() == ELFDATA2LSB ? "little endian" : "big endian") + "<br/>";
            s += QLatin1String("File type: ") + ElfPrinter::fileType(file->header()->type()) + "<br/>";
            s += QLatin1String("Machine: ") + ElfPrinter::machine(file->header()->machine()) + "<br/>";
            s += QLatin1String("OS ABI: ") + ElfPrinter::osAbi(file->osAbi()) + "<br/>";
            s += "Flags: " + QString::number(file->header()->flags()) + "<br/>";
            s += QLatin1String("Entry point: 0x") + QString::number(file->header()->entryPoint(), 16);
            if (file->header()->entryPoint() && file->symbolTable()) {
                const auto entry = file->symbolTable()->entryWithValue(file->header()->entryPoint());
                if (entry)
                    s += " (" + printSymbolName(entry) + ')';
            }
            if (file->separateDebugFile())
                s += "<br/>Separate debug file: " + file->separateDebugFile()->fileName();
            s += QLatin1String("<br/><br/>");

            s += QLatin1String("<b>Segments</b></br>");
            s += QLatin1String("<table border=\"1\" border-style=\"solid\" cellspacing=\"0\">");
            s += QLatin1String("<tr><th>Type</th><th>Flags</th><th>Offset</th><th>Virtual Address</th>");
            s += QLatin1String("<th>Physical Address</th><th>File Size</th><th>Memory Size</th><th>Alignment</th></tr>");
            foreach (auto phdr, file->segmentHeaders()) {
                s += QLatin1String("<tr>");
                s += "<td>" + ElfPrinter::segmentType(phdr->type()) + "</td>";
                s += "<td>" + ElfPrinter::segmentFlags(phdr->flags()) + "</td>";
                s += "<td>0x" + QString::number(phdr->offset(), 16) + "</td>";
                s += "<td>0x" + QString::number(phdr->virtualAddress(), 16) + "</td>";
                s += "<td>0x" + QString::number(phdr->physicalAddress(), 16) + "</td>";
                s += "<td>" + QString::number(phdr->fileSize()) + "</td>";
                s += "<td>" + QString::number(phdr->memorySize()) + "</td>";
                s += "<td>0x" + QString::number(phdr->alignment(), 16) + "</td>";
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
            return section->header()->name();
        case Qt::DecorationRole:
            if (m_column == 0 && section->file()->isSeparateDebugFile())
                return QIcon::fromTheme(QStringLiteral("package_development_debugger"));
            return {};
        case ElfModel::SizeRole:
            return QVariant::fromValue<uint64_t>(section->size());
        case ElfModel::DetailRole:
        {
            QString s(QStringLiteral("<b>Section</b><br/>"));
            s += QLatin1String("Name: ") + section->header()->name() + "<br/>";
            s += QLatin1String("Size: ") + QString::number(section->header()->size()) + " bytes<br/>";
            s += QLatin1String("Flags: ") + ElfPrinter::sectionFlags(section->header()->flags()) + "<br/>";
            s += QLatin1String("Offset: 0x") + QString::number(section->header()->sectionOffset(), 16) + "<br/>";
            s += QLatin1String("Virtual Address: 0x") + QString::number(section->header()->virtualAddress(), 16) + "<br/>";
            s += QLatin1String("Type: ") + ElfPrinter::sectionType(section->header()->type()) + "<br/>";
            if (section->header()->link())
                s += "Linked section: " + printSectionName(section->linkedSection<ElfSection>()) + "<br/>";
            if (section->header()->flags() & SHF_INFO_LINK)
                s += "Referenced section: " + printSectionName(section->file()->section<ElfSection>(section->header()->info())) + "<br/>";
            if (section->header()->entrySize()) {
                s += QLatin1String("Entries: ") + QString::number(section->header()->entryCount())
                  + " x " + QString::number(section->header()->entrySize()) + " byte<br/>";
            }
            if (section->header()->flags() & SHF_STRINGS && section->size() < 100000) { // QTextBrowser fails on too large input
                s += QLatin1String("Strings:<br/>");
                uint prevIndex = 0, index = 0;
                do {
                    if (section->rawData()[index] == 0) {
                        if (index - prevIndex > 0) {
                            const auto b = QByteArray::fromRawData(reinterpret_cast<const char*>(section->rawData() + prevIndex), index - prevIndex);
                            s += QString::number(prevIndex) + ": " + b + "<br/>";
                        }
                        prevIndex = index + 1;
                    }
                    ++index;
                } while (index < section->size());
            }
            if (strcmp(section->header()->name(), ".init") == 0 || strcmp(section->header()->name(), ".fini") == 0) {
                NavigatingDisassembler da(this);
                s += "Code:<br/><tt>" + da.disassemble(section) + "</tt>";
            }
            if (section->header()->type() == SHT_INIT_ARRAY || section->header()->type() == SHT_FINI_ARRAY) {
                const auto addrSize = section->file()->addressSize();
                s += QLatin1String("Content:<br/>");
                for (uint i = 0; i < section->header()->size() / addrSize; ++i) {
                    uint64_t value = 0;
                    memcpy(&value, section->rawData() + i * addrSize, addrSize);
                    s += QString::number(i) + ": 0x" + QString::number(value, 16);
                    if (value) {
                        const auto ref = section->file()->symbolTable()->entryWithValue(value);
                        if (ref)
                            s += QLatin1Char(' ') + printSymbolName(ref);
                    } else { // check for relocation
                        const auto relocEntry = section->file()->reverseRelocator()->find(section->header()->virtualAddress() + i * addrSize);
                        if (relocEntry) {
                            const auto sym = section->file()->symbolTable()->entryContainingValue(relocEntry->relocationTarget());
                            if (sym)
                                s += " relocated to: " + printSymbolName(sym);
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
    s += "<br/>Exported symbols: " + QString::number(symtab->exportCount());
    s += "<br/>Imported symbols: " + QString::number(symtab->importCount());
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
                    s += verDefSection->definitionForVersionIndex(versionIndex)->auxiliaryEntry(0)->name();
                }
                break;
            } else { // requirement
                const auto verReqIdx = f->indexOfSection(SHT_GNU_verneed);
                auto verReqSection = f->section<ElfGNUSymbolVersionRequirementsSection>(verReqIdx);
                assert(verReqSection);
                s += verReqSection->requirementForVersionIndex(versionIndex)->name();
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

QVariant DataVisitor::doVisit(ElfSymbolTableEntry* entry, int arg) const
{
    switch (arg) {
        case Qt::DisplayRole:
            if (*entry->name() == 0)
                return QObject::tr("<null>");
            return entry->name();
        case ElfModel::SizeRole:
            return QVariant::fromValue<uint64_t>(entry->size());
        case ElfModel::DetailRole:
        {
            QString s(QStringLiteral("<b>Symbol</b><br/>"));
            s += QLatin1String("Mangled name: ") + entry->name() + "<br/>";
            Demangler demangler;
            s += QLatin1String("Demangled name: ") + QString(demangler.demangleFull(entry->name())).toHtmlEscaped() + "<br/>";
            s += QLatin1String("Size: ") + QString::number(entry->size()) + "<br/>";
            s += QLatin1String("Value: 0x") + QString::number(entry->value(), 16) + "<br/>";
            s += QLatin1String("Bind type: ") + SymbolPrinter::bindType(entry->bindType()) + "<br/>";
            s += QLatin1String("Symbol type: ") + SymbolPrinter::symbolType(entry->type()) + "<br/>";
            s += QLatin1String("Visibility: ") + SymbolPrinter::visibility(entry->visibility()) + "<br/>";
            s += printVerSymInfo(entry);

            const auto hasValidSectionIndex = entry->hasValidSection();
            if (hasValidSectionIndex) {
                s += "Section: " + printSectionName(entry->section()) + "<br/>";
            }

            if (hasValidSectionIndex && entry->sectionHeader()->type() == SHT_NOBITS) {
                // .bss, i.e. no content to display
            } else if (entry->type() == STT_FUNC && entry->size() > 0) {
                NavigatingDisassembler da(this);
                s += QLatin1String("Code:<br/><tt>") + da.disassemble(entry) + "</tt>";
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
                            s += QString::number(i) + ": 0x" + QString::number(v, 16);
                            const auto ref = entry->symbolTable()->entryWithValue(v);
                            if (ref) {
                                s += QLatin1Char(' ') + printSymbolName(ref) + QLatin1String(" (") + Demangler::demangleFull(ref->name()) + QLatin1Char(')');
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
                            s += QString::number(i) + ": 0x" + QString::number(v, 16);
                            // vptrs point to one after the RTTI entry, which is the first virtual method, unless there is none in that
                            // case we would point past the vtable here, and thus we wont find it, so better look for the RTTI spot.
                            const auto ref = entry->symbolTable()->entryContainingValue(v - addrSize);
                            if (ref) {
                                const auto offset = v - ref->value();
                                s += QLatin1String(" entry ") + QString::number(offset / addrSize) + QLatin1String(" in ") + printSymbolName(ref);
                                s += QLatin1String(" (") + Demangler::demangleFull(ref->name()) + QLatin1Char(')');
                            }
                            s += QLatin1String("<br/>");
                        }
                        s += QLatin1String("</tt><br/>");
                        break;
                    }
                    case Demangler::SymbolType::TypeInfoName:
                        s += "Type info name: " + QByteArray((const char*)entry->data()) + "<br/>";
                        break;
                    // TODO: add other symbol types
                    default:
                        s += QStringLiteral("Data:<br/><tt>");
                        const unsigned char* data = entry->data();
                        s += QByteArray::fromRawData((const char*)data, entry->size()).toHex();
                        s += QLatin1String("</tt><br/>");
                }
            }

            const auto die = findDwarfDie(entry);
            if (die) {
                s += CodeNavigatorPrinter::sourceLocationRichText(die);
                s += QLatin1String("<br/><b>DWARF DIE</b><br/>");
                s += printDwarfDie(die);
            }
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
        s += "<br/>File name: " + section->fileName();
        s += "<br/>CRC: 0x" + QString::number(section->crc(), 16);
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
            s += QLatin1String("Tag name: ") + entry->tagName() + "<br/>";
            s += QStringLiteral("Value: ");
            switch (entry->tag()) {
                case DT_FLAGS:
                    s += DynamicSectionPrinter::flagsToDescriptions(entry->value());
                    break;
                case DT_FLAGS_1:
                    s += DynamicSectionPrinter::flags1ToDescriptions(entry->value());
                    break;
                case DT_PLTREL:
                    s += RelocationPrinter::label(entry->dynamicSection()->file()->header()->machine(), entry->value());
                    break;
                default:
                    if (entry->isStringValue()) {
                        s+= entry->stringValue();
                    } else if (entry->isAddress()) {
                        s += QLatin1String("0x") + QString::number(entry->pointer(), 16);
                        const auto secIdx = entry->dynamicSection()->file()->indexOfSectionWithVirtualAddress(entry->pointer());
                        if (secIdx >= 0) {
                            const auto section = entry->dynamicSection()->file()->section<ElfSection>(secIdx);
                            assert(section);
                            s += " (" + printSectionName(section);
                            if (section->header()->virtualAddress() < entry->pointer())
                                s += " + 0x" + QString::number(entry->pointer() - section->header()->virtualAddress(), 16);
                            s += ')';
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
        s += "Buckets: " + QString::number(section->bucketCount()) + "<br/>";
        s += "Chains: " + QString::number(section->chainCount()) + "<br/>";
        if (ElfGnuHashSection *gnuHash = dynamic_cast<ElfGnuHashSection*>(section)) {
            s += "Symbol table index: " + QString::number(gnuHash->symbolIndex()) + "<br/>";
            s += "Mask word count: " + QString::number(gnuHash->maskWordsCount()) + "<br/>";
            s += "Shift count: " + QString::number(gnuHash->shift2()) + "<br/>";
        }

        s += QLatin1String("Chain length histogram<br/>");
        const auto hist = section->histogram();
        for (int i = 0; i < hist.size(); ++i) {
            s += "&nbsp;&nbsp;" + QString::number(i) + ": " + QString::number(hist.at(i)) + "<br/>";
        }
        s += "Average collision common prefix length: " + QString::number(section->averagePrefixLength()) + "<br/>";
        return s;
    }
    return base;
}

QVariant DataVisitor::doVisit(ElfGNUSymbolVersionDefinition* verDef, int role) const
{
    switch (role) {
        case Qt::DisplayRole:
            return "version definition";
        case ElfModel::SizeRole:
            return verDef->size();
        case ElfModel::DetailRole:
        {
            QString s;
            s += "Flags: " + GnuVersionPrinter::versionFlags(verDef->flags()) + "<br/>";
            s += "Index: " + QString::number(verDef->versionIndex()) + "<br/>";
            s += "Aux count: " + QString::number(verDef->auxiliarySize()) + "<br/>";
            s += "Hash: " + QString::number(verDef->hash()) + "<br/>";
            s += "Aux offset: " + QString::number(verDef->auxiliaryOffset()) + "<br/>";
            s += "Next offset: " + QString::number(verDef->nextOffset()) + "<br/>";
            return s;
        }
    }
    return {};
}

QVariant DataVisitor::doVisit(ElfGNUSymbolVersionDefinitionAuxiliaryEntry* auxEntry, int role) const
{
    switch (role) {
        case Qt::DisplayRole:
            return auxEntry->name();
        case ElfModel::SizeRole:
            return (int)sizeof(Elf64_Verdaux);
        case ElfModel::DetailRole:
        {
            QString s;
            s += QLatin1String("Name: ") + auxEntry->name() + "<br/>";
            s += "Next: " + QString::number(auxEntry->nextAuxiliaryEntryOffset()) + "<br/>";
            return s;
        }
    }
    return {};
}

QVariant DataVisitor::doVisit(ElfGNUSymbolVersionRequirement *verNeed, int role) const
{
    switch (role) {
        case Qt::DisplayRole:
            return verNeed->fileName();
        case ElfModel::SizeRole:
            return verNeed->size();
        case ElfModel::DetailRole:
        {
            QString s;
            s += "File: " + QString(verNeed->fileName()) + "<br/>";
            s += "Aux count: " + QString::number(verNeed->auxiliarySize()) + "<br/>";
            s += "Aux offset: " + QString::number(verNeed->auxiliaryOffset()) + "<br/>";
            s += "Next offset: " + QString::number(verNeed->nextOffset()) + "<br/>";
            return s;
        }
    }
    return {};
}

QVariant DataVisitor::doVisit(ElfGNUSymbolVersionRequirementAuxiliaryEntry* auxEntry, int role) const
{
    switch (role) {
        case Qt::DisplayRole:
            return auxEntry->name();
        case ElfModel::SizeRole:
            return (int)sizeof(Elf64_Vernaux);
        case ElfModel::DetailRole:
        {
            QString s;
            s += QLatin1String("Name: ") + auxEntry->name() + "<br/>";
            s += "Hash: " + QString::number(auxEntry->hash()) + "<br/>";
            s += "Flags: " + QString::number(auxEntry->flags()) + "<br/>";
            s += "Other: " + QString::number(auxEntry->other()) + "<br/>";
            s += "Next: " + QString::number(auxEntry->nextAuxiliaryEntryOffset()) + "<br/>";
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
                return sym->name() + QStringLiteral("@got");
            return "GOT entry " + QString::number(entry->index());
        }
        case ElfModel::SizeRole:
            return entry->section()->file()->addressSize();
        case ElfModel::DetailRole:
        {
            QString s;
            s += QLatin1String("<b>GOT entry</b><br/>");
            s += "Address: 0x" + QString::number(entry->address(), 16) + "<br/><br/>";
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
            return NoteSectionPrinter::typeDisplayString(entry);
        case ElfModel::SizeRole:
            return QVariant::fromValue<quint64>(entry->size());
        case ElfModel::DetailRole:
        {
            QString s(QStringLiteral("Name: "));
            s += entry->name() + QStringLiteral("<br/>Description: ");
            if (entry->isGNUVendorNote() && entry->type() == NT_GNU_ABI_TAG) {
                s += NoteSectionPrinter::abi(entry);
            } else if (entry->isGNUVendorNote() && entry->type() == NT_GNU_GOLD_VERSION) {
                s += QByteArray(entry->descriptionData(), entry->descriptionSize());
            } else {
                s += QByteArray(entry->descriptionData(), entry->descriptionSize()).toHex();
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
                return sym->name() + QStringLiteral("@plt");
            return "PLT entry " + QString::number(entry->index());
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
                s += "Address: 0x" + QString::number(got->address(), 16) + "<br/><br/>";
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
            return RelocationPrinter::label(entry) + " 0x" + QString::number(entry->offset(), 16);
            break;
        case ElfModel::DetailRole:
            return printRelocation(entry);
    }

    return {};
}

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

QString DataVisitor::printSectionName(ElfSection* section) const
{
    const auto idx = m_model->indexForNode(section);
    const auto url = idx.data(ElfModel::NodeUrl).toUrl();

    QString s(QStringLiteral("<a href=\""));
    s += url.toEncoded();
    s += QLatin1String("\">");
    s += section->header()->name();
    s += QLatin1String("</a>");

    return s;
}

QString DataVisitor::printSymbolName(ElfSymbolTableEntry* symbol) const
{
    const auto idx = m_model->indexForNode(symbol);
    const auto url = idx.data(ElfModel::NodeUrl).toUrl();

    QString s(QStringLiteral("<a href=\""));
    s += url.toEncoded();
    s += QLatin1String("\">");
    s += symbol->name();
    s += QLatin1String("</a>");

    return s;
}

QString DataVisitor::printRelocation(ElfRelocationEntry* entry) const
{
    QString s;
    if (!entry)
        return s;

    s += QLatin1String("<b>Relocation</b><br/>");
    s += "Offset: 0x" + QString::number(entry->offset(), 16);
    const auto sym = entry->relocationTable()->file()->symbolTable()->entryContainingValue(entry->offset());
    if (sym) {
        s += QLatin1String(" (") + printSymbolName(sym) + " + 0x" + QString::number(entry->offset() - sym->value(), 16) + ')';
    } else {
        foreach (const auto shdr, entry->relocationTable()->file()->sectionHeaders()) {
            if (shdr->virtualAddress() <= entry->offset() && entry->offset() < shdr->virtualAddress() + shdr->size()) {
                s += QLatin1String(" (") + shdr->name() + " + 0x" + QString::number(entry->offset() - shdr->virtualAddress(), 16) + ')';
                break;
            }
        }
    }
    s += QLatin1String("<br/>");
    s += "Type: " + RelocationPrinter::description(entry) + " (" + RelocationPrinter::label(entry) + ")<br/>";
    const auto sourceSym = entry->symbol();
    if (sourceSym) {
        s += QLatin1String("Symbol: ") + printSymbolName(sourceSym) + "<br/>";
        s += QLatin1String("Demangled symbol: ") + QString(Demangler::demangleFull(sourceSym->name())).toHtmlEscaped() + "<br/>";
    } else {
        s += QStringLiteral("Symbol: &lt;none&gt;<br/>");
    }
    s += "Addend: 0x" + QString::number(entry->addend(), 16);
    return s;
}

QString DataVisitor::printDwarfDie(DwarfDie* die) const
{
    assert(die);
    QString s;
    s += "TAG: " + QLatin1String(die->tagName()) + "<br/>";
    s += "Name: " + QString::fromUtf8(die->name()).toHtmlEscaped() + "<br/>";
    s += "Offset: " + QString::number(die->offset()) + "<br/>";
    foreach (const auto attrType, die->attributes()) {
        const QVariant attrValue = die->attribute(attrType);
        QString attrValueStr;
        if (DwarfDie *die = attrValue.value<DwarfDie*>())
            attrValueStr = printDwarfDieName(die);
        else
            attrValueStr = attrValue.toString().toHtmlEscaped();
        s += QLatin1String(die->attributeName(attrType)) + ": " + attrValueStr + "<br/>";
    }
    return s;
}

QString DataVisitor::printDwarfDieName(DwarfDie* die) const
{
    const auto idx = m_model->indexForNode(die);
    const auto url = idx.data(ElfModel::NodeUrl).toUrl();

    QString s(QStringLiteral("<a href=\""));
    s += url.toEncoded();
    s += QLatin1String("\">");
    s += die->displayName().toHtmlEscaped();
    s += QLatin1String("</a>");

    return s;
}
