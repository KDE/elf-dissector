project(elf-dissector)

cmake_minimum_required(VERSION 2.8.9)

find_package(Qt5Widgets)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -Wall -Wextra -pedantic")
if(Qt5_POSITION_INDEPENDENT_CODE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()



include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})

set(elf_dissector_srcs
    elffile.cpp
    elfheader.cpp
    elfsectionheader.cpp
    elfsection.cpp
    elfstringtablesection.cpp
    elfsymboltablesection.cpp
    main.cpp
)

add_executable(elf-dissector ${elf_dissector_srcs})
target_link_libraries(elf-dissector Qt5::Widgets)

