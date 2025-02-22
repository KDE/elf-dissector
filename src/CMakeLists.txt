# SPDX-FileCopyrightText: 2019 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

add_subdirectory(lib)
if (NOT ELF_DISSECTOR_NO_CLI_TOOLS)
    add_subdirectory(cli)
endif()
add_subdirectory(ui)
