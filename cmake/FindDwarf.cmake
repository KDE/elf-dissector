find_path(Dwarf_INCLUDE_DIR libdwarf/libdwarf.h)

find_library(Dwarf_LIBRARY NAMES dwarf)
find_library(Elf_LIBRARY NAMES elf)
set(Dwarf_LIBS ${Dwarf_LIBRARY} ${Elf_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Dwarf DEFAULT_MSG Dwarf_LIBRARY Dwarf_INCLUDE_DIR)

include(FeatureSummary)
set_package_properties(libdwarf-devel PROPERTIES URL http://www.dwarfstd.org/
  DESCRIPTION "DWARF debug information library")
