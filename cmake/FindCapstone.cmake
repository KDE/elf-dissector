find_package(PkgConfig QUIET)
pkg_check_modules(Capstone QUIET IMPORTED_TARGET capstone)

if (Capstone_FOUND)
# if no include dirs are set, it means we should use the default include dir
if (NOT Capstone_INCLUDE_DIRS)
    set(Capstone_INCLUDE_DIRS "/usr/include")
endif()

# include paths may or may not contain the capstone subdir, make sure we always have it
# that might not be the cleanest way, but it's what newer Capstone versions seem to do, and
# it avoids conflicting with system includes triggering the -isystem bug on older GCC
find_file(_capstone_hdr NAMES "capstone.h" PATHS ${Capstone_INCLUDE_DIRS} NO_DEFAULT_PATH)
if (NOT _capstone_hdr)
    set(_capstone_new_include_dirs "")
    foreach(_i ${Capstone_INCLUDE_DIRS})
        list(APPEND _capstone_new_include_dirs "${_i}/capstone")
    endforeach()
    set(Capstone_INCLUDE_DIRS ${_capstone_new_include_dirs})
    set_target_properties(PkgConfig::Capstone PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${Capstone_INCLUDE_DIRS})
endif()
endif()

include(FeatureSummary)
set_package_properties(Capstone PROPERTIES
    URL "https://www.capstone-engine.org/"
    DESCRIPTION "Multi-platform, multi-architecture disassembly framework."
)
