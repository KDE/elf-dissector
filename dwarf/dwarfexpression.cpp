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

#include "dwarfexpression.h"
#include "dwarfleb128.h"

#include <QDebug>
#include <QString>

#include <libdwarf/dwarf.h>

static const int8_t ULEB128 = -1;
static const int8_t SLEB128 = -2;

struct opcode_t {
    uint8_t opCode;
    int8_t opSize;
    const char* name;
};

#define OP(code, argCount) { DW_OP_ ## code, argCount, "DW_OP_" #code }

static const opcode_t opcodes[] {
    OP(addr, 0),
    OP(deref, 0),
    OP(const1u, 1),
    OP(const1s, 1),
    OP(const2u, 2),
    OP(const2s, 2),
    OP(const4u, 4),
    OP(const4s, 4),
    OP(const8u, 8),
    OP(const8s, 8),
    OP(constu, ULEB128),
    OP(consts, SLEB128),
    OP(dup, 0),
    OP(drop, 0),
    OP(over, 0),
    OP(pick, 0),
    OP(swap, 0),
    OP(rot, 0),
    OP(xderef, 0),
    OP(abs, 0),
    OP(and, 0),
    OP(div, 0),
    OP(minus, 0),
    OP(mod, 0),
    OP(mul, 0),
    OP(neg, 0),
    OP(not, 0),
    OP(or, 0),
    OP(plus, 0),
    OP(plus_uconst, ULEB128),
    OP(shl, 0),
    OP(shr, 0),
    OP(shra, 0),
    OP(xor, 0),
    OP(bra, 0),
    OP(eq, 0),
    OP(ge, 0),
    OP(gt, 0),
    OP(le, 0),
    OP(lt, 0),
    OP(ne, 0),
    OP(skip, 0),
    OP(lit0, 0),
    OP(lit1, 0),
    OP(lit2, 0),
    OP(lit3, 0),
    OP(lit4, 0),
    OP(lit5, 0),
    OP(lit6, 0),
    OP(lit7, 0),
    OP(lit8, 0),
    OP(lit9, 0),
    OP(lit10, 0),
    OP(lit11, 0),
    OP(lit12, 0),
    OP(lit13, 0),
    OP(lit14, 0),
    OP(lit15, 0),
    OP(lit16, 0),
    OP(lit17, 0),
    OP(lit18, 0),
    OP(lit19, 0),
    OP(lit20, 0),
    OP(lit21, 0),
    OP(lit22, 0),
    OP(lit23, 0),
    OP(lit24, 0),
    OP(lit25, 0),
    OP(lit26, 0),
    OP(lit27, 0),
    OP(lit28, 0),
    OP(lit29, 0),
    OP(lit30, 0),
    OP(lit31, 0),
    OP(reg0, 0),
    OP(reg1, 0),
    OP(reg2, 0),
    OP(reg3, 0),
    OP(reg4, 0),
    OP(reg5, 0),
    OP(reg6, 0),
    OP(reg7, 0),
    OP(reg8, 0),
    OP(reg9, 0),
    OP(reg10, 0),
    OP(reg11, 0),
    OP(reg12, 0),
    OP(reg13, 0),
    OP(reg14, 0),
    OP(reg15, 0),
    OP(reg16, 0),
    OP(reg17, 0),
    OP(reg18, 0),
    OP(reg19, 0),
    OP(reg20, 0),
    OP(reg21, 0),
    OP(reg22, 0),
    OP(reg23, 0),
    OP(reg24, 0),
    OP(reg25, 0),
    OP(reg26, 0),
    OP(reg27, 0),
    OP(reg28, 0),
    OP(reg29, 0),
    OP(reg30, 0),
    OP(reg31, 0),
    OP(breg0, 0),
    OP(breg1, 0),
    OP(breg2, 0),
    OP(breg3, 0),
    OP(breg4, 0),
    OP(breg5, 0),
    OP(breg6, 0),
    OP(breg7, 0),
    OP(breg8, 0),
    OP(breg9, 0),
    OP(breg10, 0),
    OP(breg11, 0),
    OP(breg12, 0),
    OP(breg13, 0),
    OP(breg14, 0),
    OP(breg15, 0),
    OP(breg16, 0),
    OP(breg17, 0),
    OP(breg18, 0),
    OP(breg19, 0),
    OP(breg20, 0),
    OP(breg21, 0),
    OP(breg22, 0),
    OP(breg23, 0),
    OP(breg24, 0),
    OP(breg25, 0),
    OP(breg26, 0),
    OP(breg27, 0),
    OP(breg28, 0),
    OP(breg29, 0),
    OP(breg30, 0),
    OP(breg31, 0),
    OP(regx, 0),
    OP(fbreg, 0),
    OP(bregx, 0),
    OP(piece, 0),
    OP(deref_size, 0),
    OP(xderef_size, 0),
    OP(nop, 0),
    OP(push_object_address, 0),
    OP(call2, 0),
    OP(call4, 0),
    OP(call_ref, 0),
    OP(form_tls_address, 0),
    OP(call_frame_cfa, 0),
    OP(bit_piece, 0),
    OP(implicit_value, 0),
    OP(stack_value, 0),
    OP(GNU_push_tls_address, 0),
    OP(GNU_uninit, 0),
    OP(GNU_encoded_addr, 0),
    OP(GNU_implicit_pointer, 0),
    OP(GNU_entry_value, 0)
};

#undef OP

static const int opcodesSize = sizeof(opcodes) / sizeof(opcode_t);

static const opcode_t* opcode(uint8_t code)
{
    // TODO use the fact that opcodes is sorted...
    for (int i = 0; i < opcodesSize; ++i) {
        if (opcodes[i].opCode == code)
            return &opcodes[i];
    }
    return nullptr;
}


DwarfExpression::DwarfExpression()
{
}

DwarfExpression::DwarfExpression(Dwarf_Ptr block, Dwarf_Unsigned len)
{
    m_block = QByteArray::fromRawData(static_cast<const char*>(block), len);
}

DwarfExpression::~DwarfExpression()
{
}

QString DwarfExpression::displayString() const
{
    if (m_block.isEmpty())
        return "<empty>";

    QString s;
    for (int i = 0; i < m_block.size(); ++i) {
        const auto op = opcode(m_block.at(i));
        if (!op) {
            s += "unknown<0x" + QString::number(m_block.at(i), 16) + ">";
        } else {
            s += op->name;

            if (op->opSize >= 0) {
                // fixed size arguments
                for (int j = 0; j < op->opSize && i + j + 1 < m_block.size(); ++j) {
                    s += " 0x" + QString::number(m_block.at(i + j + 1), 16);
                }
                i += op->opSize;
            } else {
                // LEB128 encoded arguments
                int size = 0;
                if (op->opSize == ULEB128) {
                    s += " 0x" + QString::number(DwarfLEB128::decodeUnsigned(m_block.constData() + i + 1, &size), 16);
                } else if (op->opSize == SLEB128) {
                    s += " 0x" + QString::number(DwarfLEB128::decodeSigned(m_block.constData() + i + 1, &size), 16);
                } else {
                    Q_UNREACHABLE();
                }
                i += size;
            }
        }
        if (i < m_block.size() - 1)
            s += ' ';
    }

    return s;
}
