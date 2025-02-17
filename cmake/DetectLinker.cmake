# SPDX-FileCopyrightText: 2025 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

execute_process(COMMAND ${CMAKE_LINKER} --version OUTPUT_VARIABLE _out)

if (DEFINED CMAKE_LINKER_TYPE AND NOT CMAKE_LINKER_TYPE STREQUAL "DEFAULT" AND NOT CMAKE_LINKER_TYPE STREQUAL "SYSTEM")
    set(LINKER_TYPE ${CMAKE_LINKER_TYPE})
elseif (_out MATCHES "mold")
    set(LINKER_TYPE "MOLD")
elseif (_out MATCHES "gold")
    set(LINKER_TYPE "GOLD")
elseif (_out MATCHES "GNU ld")
    set(LINKER_TYPE "BFD")
elseif (_out MATCHES "[Ll][Ll][Dd]")
    set(LINKER_TYPE "LLD")
else ()
    set(LINKER_TYPE "UNKNOWN")
    message(AUTHOR_WARNING "Linker detection failed: ${_out}")
endif()

message(STATUS "Detected linker: ${LINKER_TYPE}")
