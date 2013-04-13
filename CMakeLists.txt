project(elf-dissector)

cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_INCLUDE_CURRENT_DIR on)

find_package(Qt5Widgets)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -Wall -Wextra -pedantic")
if(Qt5_POSITION_INDEPENDENT_CODE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()



include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})

set(elf_dissector_srcs
    main.cpp

    elf/elffile.cpp
    elf/elfheader.cpp
    elf/elfsectionheader.cpp
    elf/elfsection.cpp
    elf/elfstringtablesection.cpp
    elf/elfsymboltablesection.cpp
)

add_executable(elf-dissector ${elf_dissector_srcs})
target_link_libraries(elf-dissector Qt5::Widgets)

