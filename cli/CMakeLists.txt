add_executable(elf-packcheck packcheck.cpp)
target_link_libraries(elf-packcheck libelfdissector)
install(TARGETS elf-packcheck ${INSTALL_TARGETS_DEFAULT_ARGS})


add_executable(elf-depcheck depcheck.cpp)
target_link_libraries(elf-depcheck libelfdissector)
install(TARGETS elf-depcheck ${INSTALL_TARGETS_DEFAULT_ARGS})


add_executable(elf-rtticheck rtticheck.cpp)
target_link_libraries(elf-rtticheck libelfdissector)
install(TARGETS elf-rtticheck ${INSTALL_TARGETS_DEFAULT_ARGS})


add_executable(elf-optimizer optimizer.cpp)
target_link_libraries(elf-optimizer libelfdissector)
install(TARGETS elf-optimizer ${INSTALL_TARGETS_DEFAULT_ARGS})


add_executable(elf-deadcodefinder deadcode.cpp)
target_link_libraries(elf-deadcodefinder libelfdissector)
install(TARGETS elf-deadcodefinder ${INSTALL_TARGETS_DEFAULT_ARGS})
