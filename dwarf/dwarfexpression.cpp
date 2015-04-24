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
static const int8_t ADDRSIZE = -3;

struct opcode_t {
    uint8_t opCode;
    int8_t opSize;
    const char* name;
};

#define OP(code, argCount) { DW_OP_ ## code, argCount, "DW_OP_" #code }

static const opcode_t opcodes[] {
    OP(addr, ADDRSIZE),
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
    OP(pick, 1),
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
    OP(bra, 2),
    OP(eq, 0),
    OP(ge, 0),
    OP(gt, 0),
    OP(le, 0),
    OP(lt, 0),
    OP(ne, 0),
    OP(skip, 2),
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
    OP(breg0, SLEB128),
    OP(breg1, SLEB128),
    OP(breg2, SLEB128),
    OP(breg3, SLEB128),
    OP(breg4, SLEB128),
    OP(breg5, SLEB128),
    OP(breg6, SLEB128),
    OP(breg7, SLEB128),
    OP(breg8, SLEB128),
    OP(breg9, SLEB128),
    OP(breg10, SLEB128),
    OP(breg11, SLEB128),
    OP(breg12, SLEB128),
    OP(breg13, SLEB128),
    OP(breg14, SLEB128),
    OP(breg15, SLEB128),
    OP(breg16, SLEB128),
    OP(breg17, SLEB128),
    OP(breg18, SLEB128),
    OP(breg19, SLEB128),
    OP(breg20, SLEB128),
    OP(breg21, SLEB128),
    OP(breg22, SLEB128),
    OP(breg23, SLEB128),
    OP(breg24, SLEB128),
    OP(breg25, SLEB128),
    OP(breg26, SLEB128),
    OP(breg27, SLEB128),
    OP(breg28, SLEB128),
    OP(breg29, SLEB128),
    OP(breg30, SLEB128),
    OP(breg31, SLEB128),
    OP(regx, ULEB128),
    OP(fbreg, SLEB128),
    OP(bregx, 0), // TODO urgh, ULEB128 followed by SLEB128
    OP(piece, ULEB128),
    OP(deref_size, 1),
    OP(xderef_size, 1),
    OP(nop, 0),
    OP(push_object_address, 0),
    OP(call2, 2),
    OP(call4, 4),
    OP(call_ref, ADDRSIZE),
    OP(form_tls_address, 0),
    OP(call_frame_cfa, 0),
    OP(bit_piece, 0), // TODO 2x ULEB128
    OP(implicit_value, 0), // TODO ULEB128 size + data
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

DwarfExpression::DwarfExpression(Dwarf_Ptr block, Dwarf_Unsigned len, uint8_t addrSize) :
    m_addrSize(addrSize)
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
                } else if (op->opSize == ADDRSIZE) {
                    uint64_t addr = 0;
                    // TODO: endianess conversion
                    memcpy(&addr, m_block.constData() + i + 1, m_addrSize);
                    s += " 0x" + QByteArray::number(qulonglong(addr), 16);
                    size = m_addrSize;
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
