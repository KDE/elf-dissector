add_executable(demangler_test demangler_test.cpp)
target_link_libraries(demangler_test Qt5::Test Binutils::Iberty)
add_test(NAME test-demangle COMMAND demangler_test)

add_definitions(-DBINDIR="${CMAKE_BINARY_DIR}/bin/")
add_executable(elffiletest elffiletest.cpp)
target_link_libraries(elffiletest Qt5::Test libelfdissector)
add_test(NAME elffiletest COMMAND elffiletest)

add_executable(elffilesettest elffilesettest.cpp)
target_link_libraries(elffilesettest Qt5::Test libelfdissector)
add_test(NAME elffilesettest COMMAND elffilesettest)

add_executable(dwarfexpressiontest dwarfexpressiontest.cpp)
target_link_libraries(dwarfexpressiontest Qt5::Test libelfdissector)
add_test(NAME dwarfexpressiontest COMMAND dwarfexpressiontest)

add_executable(dwarfleb128test dwarfleb128test.cpp)
target_link_libraries(dwarfleb128test Qt5::Test libelfdissector)
add_test(NAME dwarfleb128test COMMAND dwarfleb128test)
