add_executable(demangler_test demangler_test.cpp)
target_link_libraries(demangler_test Qt5::Test libelfdissector)
add_test(NAME test-demangle COMMAND demangler_test)

add_definitions(-DBINDIR="${CMAKE_BINARY_DIR}/bin/")
add_executable(elffiletest elffiletest.cpp)
target_link_libraries(elffiletest Qt5::Test libelfdissector)
add_test(NAME elffiletest COMMAND elffiletest)

add_executable(elffilesettest elffilesettest.cpp)
target_link_libraries(elffilesettest Qt5::Test libelfdissector)
add_test(NAME elffilesettest COMMAND elffilesettest)

add_executable(elfnotesectiontest elfnotesectiontest.cpp)
target_link_libraries(elfnotesectiontest Qt5::Test libelfdissector)
add_test(NAME elfnotesectiontest COMMAND elfnotesectiontest)

add_executable(elfgnusymbolversioningtest elfgnusymbolversioningtest.cpp)
target_link_libraries(elfgnusymbolversioningtest Qt5::Test libelfdissector)
add_test(NAME elfgnusymbolversioningtest COMMAND elfgnusymbolversioningtest)

add_executable(dwarfexpressiontest dwarfexpressiontest.cpp)
target_link_libraries(dwarfexpressiontest Qt5::Test libelfdissector)
add_test(NAME dwarfexpressiontest COMMAND dwarfexpressiontest)

add_executable(dwarfleb128test dwarfleb128test.cpp)
target_link_libraries(dwarfleb128test Qt5::Test libelfdissector)
add_test(NAME dwarfleb128test COMMAND dwarfleb128test)

add_executable(dwarfdietest dwarfdietest.cpp)
target_link_libraries(dwarfdietest Qt5::Test libelfdissector)
add_test(NAME dwarfdietest COMMAND dwarfdietest)

add_executable(elfmodeltest elfmodeltest.cpp ${CMAKE_SOURCE_DIR}/3rdparty/qt/modeltest.cpp)
target_link_libraries(elfmodeltest Qt5::Test libelfdissectorui)
add_test(NAME elfmodeltest COMMAND elfmodeltest)

add_executable(dependencymodeltest dependencymodeltest.cpp ${CMAKE_SOURCE_DIR}/3rdparty/qt/modeltest.cpp)
target_link_libraries(dependencymodeltest Qt5::Test libelfdissectorui)
add_test(NAME dependencymodeltest COMMAND dependencymodeltest)
