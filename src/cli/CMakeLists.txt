add_executable(elf-packcheck packcheck.cpp)
target_link_libraries(elf-packcheck libelfdissector)
install(TARGETS elf-packcheck ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})


add_executable(elf-depcheck depcheck.cpp)
target_link_libraries(elf-depcheck libelfdissector)
install(TARGETS elf-depcheck ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})


add_executable(elf-rtticheck rtticheck.cpp)
target_link_libraries(elf-rtticheck libelfdissector)
install(TARGETS elf-rtticheck ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})


add_executable(elf-optimizer optimizer.cpp)
target_link_libraries(elf-optimizer libelfdissector)
install(TARGETS elf-optimizer ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})


add_executable(elf-deadcodefinder deadcode.cpp)
target_link_libraries(elf-deadcodefinder libelfdissector)
install(TARGETS elf-deadcodefinder ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
