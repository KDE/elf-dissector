set(libelfdisector_srcs
    elf/elfdynamicentry.cpp
    elf/elfdynamicsection.cpp
    elf/elffile.cpp
    elf/elffileset.cpp
    elf/elfgnudebuglinksection.cpp
    elf/elfgnuhashsection.cpp
    elf/elfgnusymbolversiontable.cpp
    elf/elfgnusymbolversiondefinition.cpp
    elf/elfgnusymbolversiondefinitionauxiliaryentry.cpp
    elf/elfgnusymbolversiondefinitionssection.cpp
    elf/elfgnusymbolversionrequirement.cpp
    elf/elfgnusymbolversionrequirementauxiliaryentry.cpp
    elf/elfgnusymbolversionrequirementssection.cpp
    elf/elfgotentry.cpp
    elf/elfgotsection.cpp
    elf/elfhashsection.cpp
    elf/elfheader.cpp
    elf/elfnoteentry.cpp
    elf/elfnotesection.cpp
    elf/elfpltentry.cpp
    elf/elfpltsection.cpp
    elf/elfrelocationentry.cpp
    elf/elfrelocationsection.cpp
    elf/elfreverserelocator.cpp
    elf/elfsectionheader.cpp
    elf/elfsection.cpp
    elf/elfsegmentheader.cpp
    elf/elfstringtablesection.cpp
    elf/elfsymboltableentry.cpp
    elf/elfsymboltablesection.cpp
    elf/elfsysvhashsection.cpp

    demangle/demangler.cpp

    disassmbler/disassembler.cpp

    dwarf/dwarfaddressranges.cpp
    dwarf/dwarfcudie.cpp
    dwarf/dwarfinfo.cpp
    dwarf/dwarfdie.cpp
    dwarf/dwarfexpression.cpp
    dwarf/dwarfleb128.cpp
    dwarf/dwarfline.cpp
    dwarf/dwarfranges.cpp

    checks/ldbenchmark.cpp
    checks/structurepackingcheck.cpp
    checks/dependenciescheck.cpp
    checks/virtualdtorcheck.cpp
    checks/deadcodefinder.cpp

    printers/dwarfprinter.cpp
    printers/dynamicsectionprinter.cpp
    printers/elfprinter.cpp
    printers/gnuversionprinter.cpp
    printers/notesectionprinter.cpp
    printers/relocationprinter.cpp
    printers/symbolprinter.cpp

    optimizers/dependencysorter.cpp
)

kde_source_files_enable_exceptions(elf/elffile.cpp)
add_library(libelfdissector STATIC ${libelfdisector_srcs})
target_link_libraries(libelfdissector PUBLIC Qt5::Core PRIVATE Binutils::Iberty Binutils::Opcodes Dwarf::Dwarf)
if (Capstone_FOUND)
    target_link_libraries(libelfdissector PRIVATE PkgConfig::Capstone)
endif()

add_subdirectory(checks)
