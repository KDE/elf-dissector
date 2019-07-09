set(CMAKE_AUTOMOC OFF)
add_executable(ldbenchmark-runner ldbenchmark-runner.c)
target_link_libraries(ldbenchmark-runner dl rt)
install(TARGETS ldbenchmark-runner ${INSTALL_TARGETS_DEFAULT_ARGS})
