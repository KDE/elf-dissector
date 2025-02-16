/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CONFIG_ELFDISSECTOR_H
#define CONFIG_ELFDISSECTOR_H

#define BINUTILS_VERSION_MAJOR ${Binutils_VERSION_MAJOR}
#define BINUTILS_VERSION_MINOR ${Binutils_VERSION_MINOR}

#define BINUTILS_VERSION ((BINUTILS_VERSION_MAJOR << 8) | BINUTILS_VERSION_MINOR)
#define BINUTILS_VERSION_CHECK(maj, min) ((maj << 8) | min)

#cmakedefine01 HAVE_DWARF
#cmakedefine HAVE_CAPSTONE

#endif
