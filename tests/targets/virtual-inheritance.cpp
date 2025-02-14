/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

struct AlignedBase {
    void* m_base;
};

struct AlignedIntermediateA : virtual public AlignedBase {
    void* m_a;
};

struct AlignedIntermediateB : virtual public AlignedBase {
    void* m_b;
};

struct AlignedDereived : public AlignedIntermediateA, public AlignedIntermediateB {
    void* m_d;
};


struct UnalignedBase {
    int m_base;
};

struct UnalignedIntermediateA : virtual public UnalignedBase {
    void* m_a;
};

struct UnalignedIntermediateB : virtual public UnalignedBase {
    bool m_b;
};

struct UnalignedDerived : public UnalignedIntermediateA, public UnalignedIntermediateB {
    short m_d;
};


int main (int, char**)
{
    // make sure the structures aren't optimized away by the compiler
    unsigned long long dummy = 0;
    #define USED(StructType) { StructType s; dummy += (unsigned long long)&s; }

    USED(AlignedBase)
    USED(AlignedIntermediateA)
    USED(AlignedIntermediateB)
    USED(AlignedDereived)
    USED(UnalignedDerived)

    return dummy;
}
