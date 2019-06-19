find_package(PkgConfig QUIET)
pkg_check_modules(Capstone QUIET IMPORTED_TARGET capstone)

include(FeatureSummary)
set_package_properties(Capstone PROPERTIES
    URL "https://www.capstone-engine.org/"
    DESCRIPTION "Multi-platform, multi-architecture disassembly framework."
)
