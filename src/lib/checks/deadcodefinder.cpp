/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "deadcodefinder.h"
#include <elf/elffileset.h>
#include <elf/elfsymboltablesection.h>
#include <elf/elfhashsection.h>
#include <elf/elfheader.h>

#include <demangle/demangler.h>

#include <elf.h>

#include <iostream>

DeadCodeFinder::DeadCodeFinder() = default;
DeadCodeFinder::DeadCodeFinder(const DeadCodeFinder&) = default;
DeadCodeFinder::~DeadCodeFinder() = default;
DeadCodeFinder& DeadCodeFinder::operator=(const DeadCodeFinder&) = default;

void DeadCodeFinder::findUnusedSymbols(ElfFileSet* fileSet)
{
    m_fileSet = fileSet;

    for (int i = 0; i < m_fileSet->size(); ++i) {
        scanUsage(m_fileSet->file(i));
    }
}

void DeadCodeFinder::scanUsage(ElfFile* file)
{
    std::cerr << "Scanning " << qPrintable(file->displayName()) << "..." << std::endl;
    for (int i = 0; i < m_fileSet->size(); ++i) {
        auto otherFile = m_fileSet->file(i);
        auto symTab = file->hash()->linkedSection<ElfSymbolTableSection>();
        if (!symTab)
            continue;
        for (uint j = 0; j < symTab->header()->entryCount(); ++j) {
            auto localSym = symTab->entry(j);
            if (localSym->size() > 0)
                continue;
            auto sym = otherFile->hash()->lookup(localSym->name());
            if (!sym)
                continue;
            m_usedSymbols[otherFile].insert(sym);
        }
    }
}

void DeadCodeFinder::setExcludePrefixes(const QStringList& excludePrefixes)
{
    m_excludePrefixes = excludePrefixes;
}

void DeadCodeFinder::dumpResults()
{
    for (int i = 0; i < m_fileSet->size(); ++i) {
        auto file = m_fileSet->file(i);
        // this only makes sense for libraries
        if (file->header()->type() == ET_EXEC)
            continue;

        bool skip = false;
        for (const auto &excludePrefix : m_excludePrefixes) {
            if (file->fileName().startsWith(excludePrefix)) {
                skip = true;
                break;
            }
        }
        if (skip)
            continue;

        std::cout << "Unreferenced exported symbols in " << qPrintable(file->displayName()) << ":" << std::endl;
        dumpResultsForFile(file);
        std::cout << std::endl;
    }
}

void DeadCodeFinder::dumpResultsForFile(ElfFile* file)
{
    const auto usedSyms = m_usedSymbols.value(file);
    const auto symTab = file->hash()->linkedSection<ElfSymbolTableSection>();
    if (!symTab)
        return;

    QList<QByteArray> unusedSyms;
    for (uint i = 0; i < symTab->header()->entryCount(); ++i) {
        auto sym = symTab->entry(i);
        if (sym->size() == 0 || sym->bindType() != STB_GLOBAL || sym->visibility() != STV_DEFAULT)
            continue;
        if (usedSyms.contains(sym))
            continue;
        unusedSyms.push_back(Demangler::demangleFull(sym->name()).constData());
    }

    std::sort(unusedSyms.begin(), unusedSyms.end());
    std::for_each(unusedSyms.constBegin(), unusedSyms.constEnd(), [](const QByteArray& sym) { std::cout << sym.constData() << std::endl; });
}
