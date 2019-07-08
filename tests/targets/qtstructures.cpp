/*
    Copyright (C) 2015 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
