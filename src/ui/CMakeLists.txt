# all code in a static library, for usage in unit tests
set(libelfdissectorui_srcs
    ${CMAKE_SOURCE_DIR}/src/3rdparty/treemap/treemap.cpp

    elfmodel/elfmodel.cpp
    elfmodel/datavisitor.cpp
    elfmodel/indexvisitor.cpp
    elfmodel/parentvisitor.cpp
    elfmodel/rowcountvisitor.cpp
    elfmodel/sectionproxymodel.cpp

    dependencymodel/dependencymodel.cpp
    dependencymodel/usedsymbolmodel.cpp
    dependencymodel/unuseddependenciesmodel.cpp
    dependencymodel/filelistmodel.cpp
    dependencymodel/fileusermodel.cpp

    issuesmodel/issuesmodel.cpp

    loadbenchmarkmodel/loadbenchmarkmodel.cpp

    typemodel/typemodel.cpp

    navigator/codenavigator.cpp
    navigator/codenavigatorprinter.cpp

    plotter/gnuplotter.cpp
    plotter/plotter.qrc

    widgets/gnuplotwidget.cpp
    widgets/sidepane.cpp

    views/dependencyview.cpp
    views/elfstructureview.cpp
    views/issuesview.cpp
    views/loadbenchmarkview.cpp
    views/sizetreemapview.cpp
    views/typeview.cpp

    mainwindow.cpp
    colorizer.cpp
)

add_library(libelfdissectorui STATIC ${libelfdissectorui_srcs})
target_link_libraries(libelfdissectorui Qt5::Widgets Dwarf::Dwarf libelfdissector KF5::ItemModels)

ecm_create_qm_loader(elfdissector_qm_loader elf-dissector_qt)
add_executable(elf-dissector
    ${elfdissector_qm_loader}
    main.cpp
)
target_link_libraries(elf-dissector libelfdissectorui)
install(TARGETS elf-dissector ${INSTALL_TARGETS_DEFAULT_ARGS})

install(PROGRAMS org.kde.elf-dissector.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.elf-dissector.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

ecm_install_icons(
  ICONS 16-apps-elf-dissector.svg  32-apps-elf-dissector.svg
  DESTINATION ${KDE_INSTALL_ICONDIR}
)
