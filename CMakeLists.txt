project(elf-dissector)

cmake_minimum_required(VERSION 2.8.12)
include(FeatureSummary)
set(CMAKE_INCLUDE_CURRENT_DIR on)
set(CMAKE_AUTOMOC on)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
enable_testing()

find_package(Qt5 COMPONENTS Widgets Test NO_MODULE REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -Wall -Wextra -pedantic")

find_package(Iberty REQUIRED)
find_package(Dwarf REQUIRED)


include_directories(
    ${Iberty_INCLUDE_DIR}
    ${Dwarf_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/3rdparty
)

set(elf_dissector_srcs
    main.cpp

    3rdparty/treemap/treemap.cpp
    3rdparty/kitemmodels/krecursivefilterproxymodel.cpp

    elf/elfdynamicentry.cpp
    elf/elfdynamicsection.cpp
    elf/elffile.cpp
    elf/elffileset.cpp
    elf/elfheader.cpp
    elf/elfsectionheader.cpp
    elf/elfsection.cpp
    elf/elfstringtablesection.cpp
    elf/elfsymboltableentry.cpp
    elf/elfsymboltablesection.cpp

    demangle/demangler.cpp

    disassmbler/disassembler.cpp

    dwarf/dwarfinfo.cpp
    dwarf/dwarfdie.cpp

    model/elfmodel.cpp
    model/datavisitor.cpp
    model/indexvisitor.cpp
    model/parentvisitor.cpp
    model/rowcountvisitor.cpp

    ui/mainwindow.cpp
    ui/colorizer.cpp
)

qt5_wrap_ui(elf_dissector_srcs
    ui/mainwindow.ui
)

add_executable(elf-dissector ${elf_dissector_srcs})
target_link_libraries(elf-dissector Qt5::Widgets ${Iberty_LIBRARY} opcodes Dwarf::Dwarf)

add_subdirectory(tests)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
