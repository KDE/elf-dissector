add_executable(single-executable single-executable.cpp)

add_executable(demangle-ast demangle-ast.cpp)
target_link_libraries(demangle-ast Qt5::Core ${Iberty_LIBRARY})
