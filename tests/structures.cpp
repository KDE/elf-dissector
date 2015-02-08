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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

struct Empty {};

struct OneByte { char m1; };

struct PackedNumbers {
    int m1;
    short m2;
    char m3;
    unsigned char m4;
};

struct NonPackedNumbers {
    char m1;
    short m2;
    char m3;
    int m4;
};

struct BitFields {
    int m1:1;
    int m2:2;
    int m3:28;
    int m4:1;
};

struct NonPackedBitFields {
    int m1:1;
    int m2;
    int m3:1;
};

struct Arrays {
    int m1[3];
    char m2[4];
};

struct NonPackedArrays {
    char m1[3];
    int m2[3];
    char m3[1];
};

struct Unions {
    union {
        int u1;
        char u2;
    } m1;
    union {
        char u1;
        short u2;
    } m2;
};

struct ConstMembers {
    const int m1 = 42;
};

struct StaticMembers {
    static const int m1 = 42;
};

struct EmptyInheritance : PackedNumbers {
};

struct NonEmptyInheritance : PackedNumbers {
    short m1;
};

int main (int, char**)
{
    // make sure the structures aren't optimized away by the compiler
    unsigned long long dummy = 0;
    #define USED(StructType) { StructType s; dummy += (unsigned long long)&s; }

    USED(Empty)
    USED(OneByte)
    USED(PackedNumbers)
    USED(NonPackedNumbers)
    USED(BitFields)
    USED(NonPackedBitFields)
    USED(Arrays)
    USED(NonPackedArrays)
    USED(Unions)
    USED(ConstMembers)
    USED(StaticMembers)
    USED(EmptyInheritance)
    USED(NonEmptyInheritance)

    return dummy;
}