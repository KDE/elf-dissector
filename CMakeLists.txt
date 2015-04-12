project(elf-dissector)

# CMake setup
cmake_minimum_required(VERSION 3.0.2)
include(FeatureSummary)
set(CMAKE_INCLUDE_CURRENT_DIR on)
set(CMAKE_AUTOMOC on)
set(CMAKE_AUTOUIC on)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
enable_testing()

# compiler setup
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GNU_SOURCE -std=c11 -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -pedantic")
if(CMAKE_SYSTEM_NAME MATCHES Linux OR CMAKE_SYSTEM_NAME STREQUAL GNU)
  if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_MODULE_LINKER_FLAGS}")
  endif()
endif()

# dependencies
find_package(Qt5 COMPONENTS Widgets Test NO_MODULE REQUIRED)

find_package(Iberty REQUIRED)
find_package(Dwarf REQUIRED)

# Installation settings
set(BIN_INSTALL_DIR "bin")
set(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)")
set(LIB_INSTALL_DIR "lib${LIB_SUFFIX}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${LIB_INSTALL_DIR})

list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}" _isSystemPlatformLibDir)
list(FIND CMAKE_C_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}" _isSystemCLibDir)
list(FIND CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}" _isSystemCxxLibDir)
if(${_isSystemPlatformLibDir} EQUAL -1 AND ${_isSystemCLibDir} EQUAL -1 AND ${_isSystemCxxLibDir} EQUAL -1)
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}")
endif()

set(INSTALL_TARGETS_DEFAULT_ARGS RUNTIME DESTINATION ${BIN_INSTALL_DIR} LIBRARY DESTINATION ${LIB_INSTALL_DIR} ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT Devel)

# build things
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/3rdparty
)

set(libelfdisector_srcs
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

    checks/ldbenchmark.cpp
    checks/structurepackingcheck.cpp
)

add_library(libelfdissector STATIC ${libelfdisector_srcs})
target_link_libraries(libelfdissector LINK_PUBLIC Qt5::Core LINK_PRIVATE Binutils::Iberty Binutils::Opcodes Dwarf::Dwarf)

set(elfpackcheck_srcs
    cli/main.cpp
)

add_executable(elf-packcheck ${elfpackcheck_srcs})
target_link_libraries(elf-packcheck libelfdissector)
install(TARGETS elf-packcheck ${INSTALL_TARGETS_DEFAULT_ARGS})

set(elf_dissector_srcs
    3rdparty/treemap/treemap.cpp
    3rdparty/kitemmodels/krecursivefilterproxymodel.cpp

    model/elfmodel.cpp
    model/datavisitor.cpp
    model/indexvisitor.cpp
    model/parentvisitor.cpp
    model/rowcountvisitor.cpp

    ui/main.cpp
    ui/mainwindow.cpp
    ui/colorizer.cpp
)

add_executable(elf-dissector ${elf_dissector_srcs})
target_link_libraries(elf-dissector Qt5::Widgets libelfdissector)
install(TARGETS elf-dissector ${INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(checks)
add_subdirectory(tests)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
