find_path(Dwarf_INCLUDE_DIR libdwarf/libdwarf.h)

find_library(Dwarf_LIBRARY NAMES dwarf)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Dwarf DEFAULT_MSG Dwarf_LIBRARY Dwarf_INCLUDE_DIR)

if(DWARF_FOUND AND NOT TARGET Dwarf::Dwarf)
    add_library(Dwarf::Dwarf UNKNOWN IMPORTED)
    set_target_properties(Dwarf::Dwarf PROPERTIES
        IMPORTED_LOCATION "${Dwarf_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${Dwarf_INCLUDE_DIR}"
    )
endif()

mark_as_advanced(Dwarf_LIBRARY Elf_LIBRARY Dwarf_INCLUDE_DIR)

include(FeatureSummary)
set_package_properties(libdwarf-devel PROPERTIES URL http://www.dwarfstd.org/
    DESCRIPTION "DWARF debug information library")
