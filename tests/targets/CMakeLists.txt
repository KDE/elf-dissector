set(CMAKE_AUTOMOC OFF)
add_executable(single-executable single-executable.c)
add_executable(structures structures.cpp)
add_executable(virtual-methods virtual-methods.cpp)
add_executable(virtual-inheritance virtual-inheritance.cpp)

add_executable(qtstructures qtstructures.cpp)
target_link_libraries(qtstructures Qt5::Core)

add_library(versioned-symbols SHARED versioned-symbols.c)
set_target_properties(versioned-symbols PROPERTIES LINK_FLAGS "-Wl,--version-script ${CMAKE_CURRENT_SOURCE_DIR}/versioned-symbols.version")
