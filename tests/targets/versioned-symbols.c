/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/


__asm__(".symver function1, function@VER1");
 __attribute__((visibility("default"))) int function1() { return 1; }

__asm__(".symver function2, function@@VER2");
 __attribute__((visibility("default"))) int function2() { return 2; }
