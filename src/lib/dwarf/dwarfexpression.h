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
