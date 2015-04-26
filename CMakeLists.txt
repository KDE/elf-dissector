# CMake setup
cmake_minimum_required(VERSION 3.0.2)
cmake_policy(SET CMP0048 NEW)
project(elf-dissector VERSION 0.0.1)
include(FeatureSummary)
set(CMAKE_INCLUDE_CURRENT_DIR on)
set(CMAKE_AUTOMOC on)
set(CMAKE_AUTOUIC on)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_CXX_VISIBILITY_PRESET "hidden")
set(CMAKE_VISIBILITY_INLINES_HIDDEN on)
enable_testing()

# version setup
set(PROJECT_VERSION_STRING "${PROJECT_VERSION}")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  find_package(Git)
  set_package_properties(Git PROPERTIES TYPE OPTIONAL PURPOSE "Determine exact build version.")
  if(GIT_FOUND)
    execute_process(
      COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE _git_revision
    )
    string(REGEX REPLACE "\n" "" _git_revision "${_git_revision}")
    set(PROJECT_VERSION_STRING "${PROJECT_VERSION_STRING} (revision: ${_git_revision})")
  endif()
endif()

# compiler setup
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GNU_SOURCE -std=c11 -Wall -Wextra -pedantic -Werror=return-type")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -pedantic -Werror=return-type")
if(CMAKE_SYSTEM_NAME MATCHES Linux OR CMAKE_SYSTEM_NAME STREQUAL GNU)
  if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_MODULE_LINKER_FLAGS}")
  endif()
endif()

# dependencies
find_package(Qt5 COMPONENTS Widgets Test NO_MODULE REQUIRED)

find_package(Iberty REQUIRED)
find_package(Dwarf REQUIRED)

# Installation settings
set(BIN_INSTALL_DIR "bin")
set(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)")
set(LIB_INSTALL_DIR "lib${LIB_SUFFIX}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${LIB_INSTALL_DIR})

list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}" _isSystemPlatformLibDir)
list(FIND CMAKE_C_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}" _isSystemCLibDir)
list(FIND CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}" _isSystemCxxLibDir)
if(${_isSystemPlatformLibDir} EQUAL -1 AND ${_isSystemCLibDir} EQUAL -1 AND ${_isSystemCxxLibDir} EQUAL -1)
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}")
endif()

set(INSTALL_TARGETS_DEFAULT_ARGS RUNTIME DESTINATION ${BIN_INSTALL_DIR} LIBRARY DESTINATION ${LIB_INSTALL_DIR} ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT Devel)

# config files
configure_file(${CMAKE_SOURCE_DIR}/config-elf-dissector.h.cmake ${CMAKE_BINARY_DIR}/config-elf-dissector.h)
configure_file(${CMAKE_SOURCE_DIR}/config-elf-dissector-version.h.cmake ${CMAKE_BINARY_DIR}/config-elf-dissector-version.h)

# build things
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/3rdparty
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_BINARY_DIR}
)

add_subdirectory(lib)
add_subdirectory(cli)
add_subdirectory(ui)
add_subdirectory(tests)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
