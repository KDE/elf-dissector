add_executable(elf-packcheck packcheck.cpp)
target_link_libraries(elf-packcheck libelfdissector)
install(TARGETS elf-packcheck ${INSTALL_TARGETS_DEFAULT_ARGS})


add_executable(elf-optimizer optimizer.cpp)
target_link_libraries(elf-optimizer libelfdissector)
install(TARGETS elf-optimizer ${INSTALL_TARGETS_DEFAULT_ARGS})
