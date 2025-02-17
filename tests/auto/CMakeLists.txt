# SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

add_executable(demangler_test demangler_test.cpp)
target_link_libraries(demangler_test Qt6::Test libelfdissector)
add_test(NAME test-demangle COMMAND demangler_test)

add_definitions(-DBINDIR="${CMAKE_BINARY_DIR}/${KDE_INSTALL_BINDIR}/")

add_executable(elffiletest elffiletest.cpp)
target_link_libraries(elffiletest Qt6::Test libelfdissector)
add_test(NAME elffiletest COMMAND elffiletest)

add_executable(elffilesettest elffilesettest.cpp)
target_link_libraries(elffilesettest Qt6::Test libelfdissector)
add_test(NAME elffilesettest COMMAND elffilesettest)

add_executable(elfsymboltabletest elfsymboltabletest.cpp)
target_link_libraries(elfsymboltabletest Qt6::Test libelfdissector)
add_test(NAME elfsymboltabletest COMMAND elfsymboltabletest)

add_executable(elfnotesectiontest elfnotesectiontest.cpp)
target_link_libraries(elfnotesectiontest Qt6::Test libelfdissector)
add_test(NAME elfnotesectiontest COMMAND elfnotesectiontest)

add_executable(elfgnusymbolversioningtest elfgnusymbolversioningtest.cpp)
if (LINKER_TYPE STREQUAL "BFD" OR LINKER_TYPE STREQUAL "GOLD")
    target_compile_definitions(elfgnusymbolversioningtest PRIVATE HAVE_SINGLE_VERSION_LINKER=0)
else()
    target_compile_definitions(elfgnusymbolversioningtest PRIVATE HAVE_SINGLE_VERSION_LINKER=1)
endif()
target_link_libraries(elfgnusymbolversioningtest Qt6::Test libelfdissector)
add_test(NAME elfgnusymbolversioningtest COMMAND elfgnusymbolversioningtest)

add_executable(elfhashtest elfhashtest.cpp)
target_link_libraries(elfhashtest Qt6::Test libelfdissector)
add_test(NAME elfhashtest COMMAND elfhashtest)

if (HAVE_DWARF)
add_executable(dwarfexpressiontest dwarfexpressiontest.cpp)
target_link_libraries(dwarfexpressiontest Qt6::Test Dwarf::Dwarf libelfdissector)
add_test(NAME dwarfexpressiontest COMMAND dwarfexpressiontest)

add_executable(dwarfleb128test dwarfleb128test.cpp)
target_link_libraries(dwarfleb128test Qt6::Test libelfdissector)
add_test(NAME dwarfleb128test COMMAND dwarfleb128test)

add_executable(dwarfdietest dwarfdietest.cpp)
target_link_libraries(dwarfdietest Qt6::Test Dwarf::Dwarf libelfdissector)
add_test(NAME dwarfdietest COMMAND dwarfdietest)
endif()

add_executable(elfmodeltest elfmodeltest.cpp)
target_link_libraries(elfmodeltest Qt6::Test libelfdissectorui)
add_test(NAME elfmodeltest COMMAND elfmodeltest)

add_executable(dependencymodeltest dependencymodeltest.cpp)
target_link_libraries(dependencymodeltest Qt6::Test libelfdissectorui)
add_test(NAME dependencymodeltest COMMAND dependencymodeltest)

if (HAVE_DWARF)
add_executable(typemodeltest typemodeltest.cpp)
target_link_libraries(typemodeltest Qt6::Test libelfdissectorui)
add_test(NAME typemodeltest COMMAND typemodeltest)
endif()
