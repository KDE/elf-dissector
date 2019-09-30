find_package(PkgConfig QUIET)
pkg_check_modules(Capstone QUIET IMPORTED_TARGET capstone)

# include paths may or may not contain the capstone subdir, make sure includes work without it
find_file(_capstone_hdr NAMES "capstone/capstone.h" PATHS ${Capstone_INCLUDE_DIRS} NO_DEFAULT_PATH)
if (NOT _capstone_hdr)
    set(_capstone_new_include_dirs "")
    foreach(_i ${Capstone_INCLUDE_DIRS})
        list(APPEND _capstone_new_include_dirs "${_i}/..")
    endforeach()
    set(Capstone_INCLUDE_DIRS ${_capstone_new_include_dirs})
    set_target_properties(PkgConfig::Capstone PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${Capstone_INCLUDE_DIRS})
endif()

include(FeatureSummary)
set_package_properties(Capstone PROPERTIES
    URL "https://www.capstone-engine.org/"
    DESCRIPTION "Multi-platform, multi-architecture disassembly framework."
)
