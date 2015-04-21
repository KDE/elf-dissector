set(elf_dissector_srcs
    ${CMAKE_SOURCE_DIR}/3rdparty/treemap/treemap.cpp
    ${CMAKE_SOURCE_DIR}/3rdparty/kitemmodels/krecursivefilterproxymodel.cpp

    model/elfmodel.cpp
    model/datavisitor.cpp
    model/indexvisitor.cpp
    model/parentvisitor.cpp
    model/rowcountvisitor.cpp

    main.cpp
    mainwindow.cpp
    colorizer.cpp
)

add_executable(elf-dissector ${elf_dissector_srcs})
target_link_libraries(elf-dissector Qt5::Widgets libelfdissector)
install(TARGETS elf-dissector ${INSTALL_TARGETS_DEFAULT_ARGS})
