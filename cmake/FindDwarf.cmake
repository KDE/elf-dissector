include(CheckCSourceCompiles)
include(CMakePushCheckState)

find_path(Dwarf_INCLUDE_DIR libdwarf.h PATH_SUFFIXES libdwarf)

find_library(Dwarf_LIBRARY NAMES dwarf)

# test if libdwarf requires libelf to link
cmake_push_check_state()
set(CMAKE_REQUIRED_INCLUDES ${Dwarf_INCLUDE_DIR})
set(CMAKE_REQUIRED_LIBRARIES ${Dwarf_LIBRARY})
check_c_source_compiles("
#include <libdwarf.h>
int main(int argc, char** argv) {
    Dwarf_Die die;
    return dwarf_tag(die, 0, 0);
}" DWARF_LINKS_WITHOUT_LIBELF)

if(NOT DWARF_LINKS_WITHOUT_LIBELF)
    find_library(elf_LIBRARY NAMES elf)
    if (NOT elf_LIBRARY)
       message(WARNING "elf library not found")
    endif()
    set(CMAKE_REQUIRED_LIBRARIES ${Dwarf_LIBRARY} ${elf_LIBRARY})
    check_c_source_compiles("
    #include <libdwarf.h>
    int main(int argc, char** argv) {
        Dwarf_Die die;
        return dwarf_tag(die, 0, 0);
    }" DWARF_NEEDS_LIBELF)
endif()
cmake_pop_check_state()

if(NOT DWARF_LINKS_WITHOUT_LIBELF AND NOT DWARF_NEEDS_LIBELF)
    message(FATAL_ERROR "Can't get libdwarf to link!?")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Dwarf DEFAULT_MSG Dwarf_LIBRARY Dwarf_INCLUDE_DIR)

if(DWARF_FOUND AND NOT TARGET Dwarf::Dwarf)
    add_library(Dwarf::Dwarf UNKNOWN IMPORTED)
    set_target_properties(Dwarf::Dwarf PROPERTIES
        IMPORTED_LOCATION "${Dwarf_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${Dwarf_INCLUDE_DIR}"
    )
    if(DWARF_NEEDS_LIBELF)
        set_target_properties(Dwarf::Dwarf PROPERTIES INTERFACE_LINK_LIBRARIES ${elf_LIBRARY})
    endif()
endif()

mark_as_advanced(Dwarf_LIBRARY Elf_LIBRARY Dwarf_INCLUDE_DIR)

include(FeatureSummary)
set_package_properties(libdwarf-devel PROPERTIES URL http://www.dwarfstd.org/
    DESCRIPTION "DWARF debug information library")
