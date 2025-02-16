# SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

add_executable(demangle-ast demangle-ast.cpp)
target_link_libraries(demangle-ast Qt6::Core Binutils::Iberty)
