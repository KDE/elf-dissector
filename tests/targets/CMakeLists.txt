set(CMAKE_AUTOMOC OFF)
add_executable(single-executable single-executable.c)
add_executable(structures structures.cpp)

add_executable(virtual-methods virtual-methods.cpp)
if (CMAKE_COMPILER_IS_GNUCXX)
    # we explicitly want this error in the test binary, so silence the corresponding warning
    target_compile_options(virtual-methods PRIVATE "-Wno-non-virtual-dtor")
endif()

add_executable(virtual-inheritance virtual-inheritance.cpp)

add_executable(qtstructures qtstructures.cpp)
target_link_libraries(qtstructures Qt5::Core)

add_library(versioned-symbols SHARED versioned-symbols.c)
set_target_properties(versioned-symbols PROPERTIES LINK_FLAGS "-Wl,--version-script ${CMAKE_CURRENT_SOURCE_DIR}/versioned-symbols.version")
