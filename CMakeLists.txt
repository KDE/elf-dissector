project(elf-dissector)

cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_INCLUDE_CURRENT_DIR on)
set(CMAKE_AUTOMOC on)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(Qt5Widgets)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -Wall -Wextra -pedantic")
if(Qt5_POSITION_INDEPENDENT_CODE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

find_package(Iberty REQUIRED)


include_directories(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Iberty_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/3rdparty
)

set(elf_dissector_srcs
    main.cpp

    3rdparty/treemap/treemap.cpp

    elf/elffile.cpp
    elf/elffileset.cpp
    elf/elfheader.cpp
    elf/elfsectionheader.cpp
    elf/elfsection.cpp
    elf/elfstringtablesection.cpp
    elf/elfsymboltableentry.cpp
    elf/elfsymboltablesection.cpp

    demangle/demangler.cpp

    model/elfmodel.cpp
    model/datavisitor.cpp
    model/indexvisitor.cpp
    model/parentvisitor.cpp
    model/rowcountvisitor.cpp

    ui/mainwindow.cpp
    ui/colorizer.cpp
)

qt5_wrap_ui(elf_dissector_srcs
    ui/mainwindow.ui
)

add_executable(elf-dissector ${elf_dissector_srcs})
target_link_libraries(elf-dissector Qt5::Widgets ${Iberty_LIBRARY})