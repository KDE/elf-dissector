# CMake setup
cmake_minimum_required(VERSION 3.1)
if(POLICY CMP0063)
  cmake_policy(SET CMP0063 NEW)
endif()
project(elf-dissector VERSION 0.0.1)

find_package(ECM 5.38 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMSetupVersion)
include(ECMPoQmTools)
include(ECMInstallIcons)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)

set(CMAKE_AUTOUIC on)
set(CMAKE_AUTORCC on)
set(CMAKE_CXX_VISIBILITY_PRESET "hidden")
set(CMAKE_VISIBILITY_INLINES_HIDDEN on)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
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
if(CMAKE_SYSTEM_NAME MATCHES Linux OR CMAKE_SYSTEM_NAME STREQUAL GNU)
  if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(EXTRA_LINKER_FLAGS "-Wl,--hash-style=both")
    set(CMAKE_EXE_LINKER_FLAGS "${EXTRA_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS "${EXTRA_LINKER_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS "${EXTRA_LINKER_FLAGS} ${CMAKE_MODULE_LINKER_FLAGS}")
  endif()
endif()

# dependencies
find_package(Qt5 5.11 COMPONENTS Widgets Test NO_MODULE REQUIRED)
find_package(KF5ItemModels NO_MODULE REQUIRED)

find_package(Iberty REQUIRED)
find_package(Dwarf REQUIRED)
if (NOT OPCODES_FOUND)
    message(FATAL_ERROR "Binutils::Opcodes library not found")
endif()
find_package(Capstone)
if (Capstone_FOUND)
    set(HAVE_CAPSTONE TRUE)
endif()
find_package(Gnuplot QUIET)
set_package_properties(Gnuplot PROPERTIES
    DESCRIPTION "a command-line driven graphing utility"
    URL "https://www.gnuplot.info/"
    PURPOSE "Required for the Performance benchmark tool"
    TYPE RUNTIME
)

# config files
configure_file(${CMAKE_SOURCE_DIR}/config-elf-dissector.h.cmake ${CMAKE_BINARY_DIR}/config-elf-dissector.h)
configure_file(${CMAKE_SOURCE_DIR}/config-elf-dissector-version.h.cmake ${CMAKE_BINARY_DIR}/config-elf-dissector-version.h)

# build things
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/3rdparty
    ${CMAKE_SOURCE_DIR}/src/lib
    ${CMAKE_BINARY_DIR}
)

add_subdirectory(src)
add_subdirectory(tests)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
