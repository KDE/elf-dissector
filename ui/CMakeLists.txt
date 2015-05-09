# all code in a static library, for usage in unit tests
set(libelfdissectorui_srcs
    ${CMAKE_SOURCE_DIR}/3rdparty/treemap/treemap.cpp
    ${CMAKE_SOURCE_DIR}/3rdparty/kitemmodels/krecursivefilterproxymodel.cpp

    elfmodel/elfmodel.cpp
    elfmodel/datavisitor.cpp
    elfmodel/indexvisitor.cpp
    elfmodel/parentvisitor.cpp
    elfmodel/rowcountvisitor.cpp
    elfmodel/sectionproxymodel.cpp

    dependencymodel/dependencymodel.cpp

    typemodel/typemodel.cpp

    navigator/codenavigator.cpp
    navigator/codenavigatorprinter.cpp

    plotter/gnuplotter.cpp
    plotter/plotter.qrc

    widgets/gnuplotwidget.cpp

    views/dependencyview.cpp
    views/elfstructureview.cpp
    views/loadbenchmarkview.cpp
    views/sizetreemapview.cpp
    views/typeview.cpp

    mainwindow.cpp
    colorizer.cpp
)

add_library(libelfdissectorui STATIC ${libelfdissectorui_srcs})
target_link_libraries(libelfdissectorui Qt5::Widgets libelfdissector)

add_executable(elf-dissector main.cpp)
target_link_libraries(elf-dissector libelfdissectorui)
install(TARGETS elf-dissector ${INSTALL_TARGETS_DEFAULT_ARGS})
