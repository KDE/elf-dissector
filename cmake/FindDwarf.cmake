# SPDX-FileCopyrightText: 2019 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

find_package(PkgConfig QUIET)
pkg_check_modules(PKG_dwarf QUIET libdwarf)

set(Dwarf_VERSION ${PKG_dwarf_VERSION})

find_path(Dwarf_INCLUDE_DIR
    NAMES libdwarf.h
    PATH_SUFFIXES "libdwarf" "libdwarf-0"
    HINTS ${PKG_dwarf_INCLUDEDIR} ${PKG_dwarf_INCLUDE_DIR}
)

find_library(Dwarf_LIBRARY
    NAMES dwarf
    HINTS ${PKG_dwarf_LIBDIR} ${PKG_dwarf_LIBRARY_DIRS}
)

# test if we found the real libdwarf of the FreeBSD variant
# of the same name but with different API...
include(CheckCSourceCompiles)
include(CMakePushCheckState)
cmake_push_check_state()
set(CMAKE_REQUIRED_INCLUDES ${Dwarf_INCLUDE_DIR})
set(CMAKE_REQUIRED_LIBRARIES ${Dwarf_LIBRARY})
check_c_source_compiles("
#include <libdwarf.h>
int main(int argc, char** argv) {
    Dwarf_Obj_Access_Interface_a objAccessIface;
    return 0;
}" Dwarf_ACTUALLY_BUILDS)
cmake_pop_check_state()

if (Dwarf_ACTUALLY_BUILDS)
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(Dwarf
        FOUND_VAR Dwarf_FOUND
        REQUIRED_VARS Dwarf_LIBRARY Dwarf_INCLUDE_DIR
        VERSION_VAR Dwarf_VERSION
    )

    if(Dwarf_FOUND AND NOT TARGET Dwarf::Dwarf)
        add_library(Dwarf::Dwarf UNKNOWN IMPORTED)
        set_target_properties(Dwarf::Dwarf PROPERTIES
            IMPORTED_LOCATION "${Dwarf_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${Dwarf_INCLUDE_DIR}"
        )
    endif()
endif()

mark_as_advanced(Dwarf_LIBRARY Dwarf_INCLUDE_DIR Dwarf_VERSION)

include(FeatureSummary)
set_package_properties(libdwarf-devel PROPERTIES URL http://www.dwarfstd.org/
    DESCRIPTION "DWARF debug information library")
