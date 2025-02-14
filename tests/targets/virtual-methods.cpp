/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

struct Base {
    virtual void pure() = 0;
    virtual void baseOnly() {};
    virtual void overridden() {};
};

struct Derived : public Base {
    void pure() override {};
    void overridden() override {};
};

struct BaseWithoutVirtuals {
    void *m_b;
};

struct DerivedFromBaseWithoutVTable : BaseWithoutVirtuals {
    virtual void function() {};
    void *m_d;
};

int main (int, char**)
{
    // make sure the structures aren't optimized away by the compiler
    unsigned long long dummy = 0;
    #define USED(StructType) { StructType s; dummy += (unsigned long long)&s; }

    USED(Derived);
    USED(DerivedFromBaseWithoutVTable);

    return dummy;
}
