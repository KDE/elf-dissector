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

#include "dwarfexpression.h"
#include "dwarfleb128.h"

#include <QDebug>
#include <QString>
#include <QtEndian>

#include <dwarf.h>

#include <cassert>

enum class OperandType : int8_t {
    NONE,
    U1,
    S1,
    U2,
    S2,
    U4,
    S4,
    U8,
    S8,
    ADDR,
    ULEB128,
    SLEB128,
    ULEB128ULEB128,
    ULEB128SLEB128,
    ULEB128BLOCK
};

struct opcode_t {
    uint8_t opCode;
    OperandType opType;
    const char* name;
};

#define OP(code, opType) { DW_OP_ ## code, OperandType:: opType, "DW_OP_" #code }

static const opcode_t opcodes[] {
    OP(addr, ADDR),
    OP(deref, NONE),
    OP(const1u, U1),
    OP(const1s, S1),
    OP(const2u, U2),
    OP(const2s, S2),
    OP(const4u, U4),
    OP(const4s, S4),
    OP(const8u, U8),
    OP(const8s, S8),
    OP(constu, ULEB128),
    OP(consts, SLEB128),
    OP(dup, NONE),
    OP(drop, NONE),
    OP(over, NONE),
    OP(pick, U1),
    OP(swap, NONE),
    OP(rot, NONE),
    OP(xderef, NONE),
    OP(abs, NONE),
    OP(and, NONE),
    OP(div, NONE),
    OP(minus, NONE),
    OP(mod, NONE),
    OP(mul, NONE),
    OP(neg, NONE),
    OP(not, NONE),
    OP(or, NONE),
    OP(plus, NONE),
    OP(plus_uconst, ULEB128),
    OP(shl, NONE),
    OP(shr, NONE),
    OP(shra, NONE),
    OP(xor, NONE),
    OP(bra, S2),
    OP(eq, NONE),
    OP(ge, NONE),
    OP(gt, NONE),
    OP(le, NONE),
    OP(lt, NONE),
    OP(ne, NONE),
    OP(skip, S2),
    OP(lit0, NONE),
    OP(lit1, NONE),
    OP(lit2, NONE),
    OP(lit3, NONE),
    OP(lit4, NONE),
    OP(lit5, NONE),
    OP(lit6, NONE),
    OP(lit7, NONE),
    OP(lit8, NONE),
    OP(lit9, NONE),
    OP(lit10, NONE),
    OP(lit11, NONE),
    OP(lit12, NONE),
    OP(lit13, NONE),
    OP(lit14, NONE),
    OP(lit15, NONE),
    OP(lit16, NONE),
    OP(lit17, NONE),
    OP(lit18, NONE),
    OP(lit19, NONE),
    OP(lit20, NONE),
    OP(lit21, NONE),
    OP(lit22, NONE),
    OP(lit23, NONE),
    OP(lit24, NONE),
    OP(lit25, NONE),
    OP(lit26, NONE),
    OP(lit27, NONE),
    OP(lit28, NONE),
    OP(lit29, NONE),
    OP(lit30, NONE),
    OP(lit31, NONE),
    OP(reg0, NONE),
    OP(reg1, NONE),
    OP(reg2, NONE),
    OP(reg3, NONE),
    OP(reg4, NONE),
    OP(reg5, NONE),
    OP(reg6, NONE),
    OP(reg7, NONE),
    OP(reg8, NONE),
    OP(reg9, NONE),
    OP(reg10, NONE),
    OP(reg11, NONE),
    OP(reg12, NONE),
    OP(reg13, NONE),
    OP(reg14, NONE),
    OP(reg15, NONE),
    OP(reg16, NONE),
    OP(reg17, NONE),
    OP(reg18, NONE),
    OP(reg19, NONE),
    OP(reg20, NONE),
    OP(reg21, NONE),
    OP(reg22, NONE),
    OP(reg23, NONE),
    OP(reg24, NONE),
    OP(reg25, NONE),
    OP(reg26, NONE),
    OP(reg27, NONE),
    OP(reg28, NONE),
    OP(reg29, NONE),
    OP(reg30, NONE),
    OP(reg31, NONE),
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
    OP(bregx, ULEB128SLEB128),
    OP(piece, ULEB128),
    OP(deref_size, U1),
    OP(xderef_size, U1),
    OP(nop, NONE),
    OP(push_object_address, NONE),
    OP(call2, U2),
    OP(call4, U4),
    OP(call_ref, ADDR),
    OP(form_tls_address, NONE),
    OP(call_frame_cfa, NONE),
    OP(bit_piece, ULEB128ULEB128),
    OP(implicit_value, ULEB128BLOCK),
    OP(stack_value, NONE),
    OP(GNU_push_tls_address, NONE),
    OP(GNU_uninit, NONE),
    OP(GNU_encoded_addr, NONE),
    OP(GNU_implicit_pointer, NONE),
    OP(GNU_entry_value, NONE)
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

DwarfExpression::DwarfExpression() : m_addrSize(0)
{
}

DwarfExpression::DwarfExpression(Dwarf_Ptr block, Dwarf_Unsigned len, uint8_t addrSize) :
    m_addrSize(addrSize)
{
    assert(m_addrSize == 4 || m_addrSize == 8);
    m_block = QByteArray::fromRawData(static_cast<const char*>(block), len);
}

QString DwarfExpression::displayString() const
{
    if (m_block.isEmpty())
        return QStringLiteral("<empty>");

    QString s;
    for (int i = 0; i < m_block.size(); ++i) {
        const auto op = opcode(m_block.at(i));
        if (!op) {
            s += QLatin1String("unknown<0x") + QString::number((uint8_t)m_block.at(i), 16) + QLatin1Char('>');
        } else {
            s += op->name;
            int size = 0;

            // TODO correct endianess conversion
            #define NUM2STR(type) \
            {\
                size = sizeof(type); \
                assert(m_block.size() > i + size); \
                const auto value = readNumber<type>(i + 1); \
                s += ' ' + QString::number(value); \
                break; \
            }

            switch (op->opType) {
                case OperandType::NONE:
                    break;

                case OperandType::U1:
                    NUM2STR(uint8_t)
                case OperandType::U2:
                    NUM2STR(uint16_t)
                case OperandType::U4:
                    NUM2STR(uint32_t)
                case OperandType::U8:
                    NUM2STR(quint64) // uint64_t != quint64...

                case OperandType::S1:
                    NUM2STR(int8_t)
                case OperandType::S2:
                    NUM2STR(int16_t)
                case OperandType::S4:
                    NUM2STR(int32_t)
                case OperandType::S8:
                    NUM2STR(quint64)

                case OperandType::ADDR:
                {
                    uint64_t addr = 0;
                    // TODO: endianess conversion
                    memcpy(&addr, m_block.constData() + i + 1, m_addrSize);
                    s += " 0x" + QByteArray::number(qulonglong(addr), 16);
                    size = m_addrSize;
                    break;
                }

                case OperandType::ULEB128:
                    s += ' ' + QString::number(DwarfLEB128::decodeUnsigned(m_block.constData() + i + 1, &size));
                    break;
                case OperandType::SLEB128:
                    s += ' ' + QString::number(DwarfLEB128::decodeSigned(m_block.constData() + i + 1, &size));
                    break;
                case OperandType::ULEB128ULEB128:
                {
                    // TODO
                    assert(!"not implemented yet");
                    break;
                }
                case OperandType::ULEB128SLEB128:
                    // TODO
                    assert(!"not implemented yet");
                    break;
                case OperandType::ULEB128BLOCK:
                    // TODO
                    assert(!"not implemented yet");
                    break;
            }

            #undef NUM2STR
            i += size;
        }
        if (i < m_block.size() - 1)
            s += ' ';
    }

    return s;
}

void DwarfExpression::push(uint64_t value)
{
    m_stack.push(value);
}

uint64_t DwarfExpression::top() const
{
    return m_stack.top();
}

uint64_t DwarfExpression::pop()
{
    return m_stack.pop();
}

bool DwarfExpression::evaluateSimple()
{
    for (int i = 0; i < m_block.size();++i) {
        const auto consumed = evaluateOne(i);
        if (consumed < 0)
            return false;
        i += consumed;
    }
    return !m_stack.isEmpty();
}

template <typename T> T DwarfExpression::readNumber(int index) const
{
    return qFromLittleEndian<T>(reinterpret_cast<const unsigned char*>(m_block.constData() + index));
}

int DwarfExpression::evaluateOne(int i)
{
    const uint8_t code = m_block.at(i);
    switch (code) {
        case DW_OP_addr:
            if (m_addrSize == 4)
                push(readNumber<uint32_t>(i + 1));
            else
                push(readNumber<quint64>(i + 1));
            return m_addrSize;
        case DW_OP_const1u:
            push(readNumber<uint8_t>(i + 1));
            return 1;
        case DW_OP_const1s:
            push(readNumber<int8_t>(i + 1));
            return 1;
        case DW_OP_const2u:
            push(readNumber<uint16_t>(i + 1));
            return 2;
        case DW_OP_const2s:
            push(readNumber<int16_t>(i + 1));
            return 2;
        case DW_OP_const4u:
            push(readNumber<uint32_t>(i + 1));
            return 4;
        case DW_OP_const4s:
            push(readNumber<int32_t>(i + 1));
            return 4;
        case DW_OP_const8u:
        case DW_OP_const8s: // signed vs. unsigned doesn't matter here, we copy everything anyway
            push(readNumber<quint64>(i + 1));
            return 8;
        case DW_OP_constu:
        {
            int size = 0;
            push(DwarfLEB128::decodeUnsigned(m_block.constData() + i +  1, &size));
            return size;
        }
        case DW_OP_consts:
        {
            int size = 0;
            push(DwarfLEB128::decodeSigned(m_block.constData() + i +  1, &size));
            return size;
        }

        case DW_OP_dup:
            push(top());
            return 0;

        case DW_OP_minus:
        {
            const auto o1 = pop();
            const auto o2 = pop();
            push(o2 - o1);
            return 0;
        }
        case DW_OP_plus:
            push(pop() + pop());
            return 0;
    }

    if (code >= DW_OP_lit0 && code <= DW_OP_lit31) {
        push(code - DW_OP_lit0);
        return 0;
    }

    const auto op = opcode(code);
    if (!op) {
        qWarning() << "unknown opcode: " << code;
    } else {
        qWarning() << "opcode" << op->name << "not yet implemented";
    }

    return -1;
}
