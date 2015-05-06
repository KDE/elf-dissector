set(CMAKE_AUTOMOC OFF)
add_executable(single-executable single-executable.c)
add_executable(structures structures.cpp)
add_executable(virtual-methods virtual-methods.cpp)
add_executable(virtual-inheritance virtual-inheritance.cpp)

add_executable(qtstructures qtstructures.cpp)
target_link_libraries(qtstructures Qt5::Core)
