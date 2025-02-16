# SPDX-FileCopyrightText: 2019 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

set(CMAKE_AUTOMOC OFF)
add_executable(ldbenchmark-runner ldbenchmark-runner.c)
target_link_libraries(ldbenchmark-runner dl rt)
install(TARGETS ldbenchmark-runner ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
