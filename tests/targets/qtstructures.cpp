/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <QSharedData>

class QValueTypePrivateUnpacked : public QSharedData
{
    // 4 byte padding
    void *m1;
    int m2;
};

class QValueTypePrivatePacked : public QSharedData
{
    int m1;
    void *m2;
};


int main (int, char**)
{
    // make sure the structures aren't optimized away by the compiler
    unsigned long long dummy = 0;
    #define USED(StructType) { StructType s; dummy += (unsigned long long)&s; }

    USED(QValueTypePrivateUnpacked)
    USED(QValueTypePrivatePacked)

    return dummy;
}
