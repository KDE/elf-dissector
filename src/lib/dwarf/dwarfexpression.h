/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DWARFEXPRESSION_H
#define DWARFEXPRESSION_H

#include <QByteArray>
#include <QMetaType>
#include <QStack>

#include <libdwarf.h>

class QString;

/** Handles DW_FORM_exprloc attribute values. */
class DwarfExpression
{
public:
    DwarfExpression();
    DwarfExpression(const DwarfExpression&) = default;
    explicit DwarfExpression(Dwarf_Ptr block, Dwarf_Unsigned len, uint8_t addrSize);
    ~DwarfExpression() = default;

    DwarfExpression& operator=(const DwarfExpression&) = default;

    QString displayString() const;

    /** Push a value to the evaluation stack.
     *  Depending on the type of the expression, this might be needed before evaluation.
     */
    void push(uint64_t value);
    /** Returns the top-most value of the evaluation stack.
     *  This is typically used to retrieve the result of an evaluation.
     */
    uint64_t top() const;
    uint64_t pop();

    /** Evaluate a simple expression, ie. one that results in one result value.
     *  @return @c true on success, @c false otherwise, the stack contains invalid data in that case.
     */
    bool evaluateSimple();

private:
    template <typename T> T readNumber(int index) const;
    int evaluateOne(int index);

    QByteArray m_block;
    uint8_t m_addrSize;
    QStack<uint64_t> m_stack;
};

Q_DECLARE_METATYPE(DwarfExpression)

#endif // DWARFEXPRESSION_H
