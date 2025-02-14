/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

const char roData[] = "HELLO WORLD!";

int function(int i)
{
    return i + 42;
}

int main (int argc, __attribute__((unused)) char **argv)
{
    return argc + function(0) + roData[2];
}
