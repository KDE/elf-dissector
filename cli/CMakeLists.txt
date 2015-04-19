set(elfpackcheck_srcs
    main.cpp
)

add_executable(elf-packcheck ${elfpackcheck_srcs})
target_link_libraries(elf-packcheck libelfdissector)
install(TARGETS elf-packcheck ${INSTALL_TARGETS_DEFAULT_ARGS})
