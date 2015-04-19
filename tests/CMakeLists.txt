add_executable(single-executable single-executable.cpp)
add_executable(structures structures.cpp)
add_executable(virtual-inheritance virtual-inheritance.cpp)

add_executable(demangle-ast demangle-ast.cpp)
target_link_libraries(demangle-ast Qt5::Core ${Iberty_LIBRARY})

add_executable(demangler_test demangler_test.cpp)
target_link_libraries(demangler_test Qt5::Test ${Iberty_LIBRARY})
add_test(NAME test-demangle COMMAND demangler_test)

add_definitions(-DBINDIR="${CMAKE_BINARY_DIR}/bin/")
add_executable(elffiletest elffiletest.cpp)
target_link_libraries(elffiletest Qt5::Test libelfdissector)
add_test(NAME elffiletest COMMAND elffiletest)

add_executable(elffilesettest elffilesettest.cpp)
target_link_libraries(elffilesettest Qt5::Test libelfdissector)
add_test(NAME elffilesettest COMMAND elffilesettest)
