/*
    Copyright (C) 2013-2014 Volker Krause <vkrause@kde.org>

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

#include <config-elf-dissector.h>
#include "demangler.h"

#include <QDebug>
#include <QScopedValueRollback>

// workarounds for conflicting declaration in libiberty.h
#define HAVE_DECL_BASENAME 1
#define HAVE_DECL_ASPRINTF 1
#define HAVE_DECL_VASPRINTF 1

#include <demangle.h>


QVector<QByteArray> Demangler::demangle(const char* name)
{
    QScopedValueRollback<const char*> mangledName(m_mangledName, name);

    void *memory = nullptr;
    demangle_component *component = cplus_demangle_v3_components(name, DMGL_PARAMS | DMGL_ANSI | DMGL_TYPES | DMGL_VERBOSE, &memory);

    QVector<QByteArray> result;
    if (!memory || !component) { // demangle failed, likely not mangled
        result.push_back(name);
        return result;
    }

    reset();
    handleNameComponent(component, result);
    free(memory);
    return result;
}

QByteArray Demangler::demangleFull(const char* name)
{
    void *memory = nullptr;
    demangle_component *component = cplus_demangle_v3_components(name, DMGL_PARAMS | DMGL_ANSI | DMGL_TYPES | DMGL_VERBOSE, &memory);

    size_t size;
    char * fullName = cplus_demangle_print(DMGL_PARAMS | DMGL_ANSI | DMGL_TYPES | DMGL_VERBOSE, component, strlen(name), &size);
    const QByteArray b(fullName);

    free(fullName);
    free(memory);

    if (b.isEmpty())
        return name;
    return b;
}

void Demangler::reset()
{
    m_inArgList = false;
    m_templateParamIndex = 0;
    m_templateParams.clear();
    m_pendingPointer = false;
    m_pendingReference = false;
}

// not in a public binutils header, but needed anyway
struct demangle_operator_info
{
    const char *type;
    const char *name;
    int len;
    int args;
};

struct demangle_builtin_type_info {
    const char *name;
    int len;
    const char *java_name;
    int java_len;
    /*enum d_builtin_type_print*/ int print;
};

static QByteArray join(const QVector<QByteArray> &v, const QByteArray &sep)
{
    QByteArray res;
    for (auto it  = v.begin(); it != v.end(); ++it) {
        if (it != v.begin())
            res += sep;
        res += *it;
    }
    return res;
}

void Demangler::handleNameComponent(demangle_component* component, QVector< QByteArray >& nameParts)
{
    // TODO: complete the component types
    switch (component->type) {
        case DEMANGLE_COMPONENT_NAME:
            nameParts.push_back(QByteArray(component->u.s_name.s, component->u.s_name.len));
            break;
        case DEMANGLE_COMPONENT_QUAL_NAME:
            handleNameComponent(component->u.s_binary.left, nameParts);
            handleNameComponent(component->u.s_binary.right, nameParts);
            if (m_inArgList) {
                const QByteArray name = nameParts.takeLast();
                const QByteArray ns = nameParts.takeLast();
                nameParts.push_back(ns + "::" + name);
            }
            break;
        case DEMANGLE_COMPONENT_LOCAL_NAME:
            handleNameComponent(component->u.s_binary.left, nameParts);
            handleNameComponent(component->u.s_binary.right, nameParts);
            if (m_inArgList) {
                const QByteArray name = nameParts.takeLast();
                const QByteArray ns = nameParts.takeLast();
                nameParts.push_back(ns + "::" + name);
            }
            break;
        case DEMANGLE_COMPONENT_TYPED_NAME:
        {
            // template parameters are indexed per enclosing type name, so push that on the stack here
            QScopedValueRollback<int> indexRestter(m_templateParamIndex, 0);
            QScopedValueRollback<QHash<int, QByteArray>> paramsResetter(m_templateParams);
            QScopedValueRollback<bool> shouldIndexResetter(m_indexTemplateArgs, true);
            QScopedValueRollback<QByteArray> modifierResetter(m_modifiers);
            m_templateParams.clear();
            m_modifiers.clear();

            // left is the name of the function, right is the return type (ignored here) and arguments
            handleNameComponent(component->u.s_binary.left, nameParts);
            QVector<QByteArray> args;
            handleNameComponent(component->u.s_binary.right, args);
            if (!nameParts.isEmpty() && !args.isEmpty())
                nameParts.last().append(args.last() + m_modifiers);
            break;
        }
        case DEMANGLE_COMPONENT_TEMPLATE:
        {
            {
                QScopedValueRollback<bool> indexRestter(m_indexTemplateArgs, false);
                handleNameComponent(component->u.s_binary.left, nameParts);
            }
            QVector<QByteArray> args;
            handleNameComponent(component->u.s_binary.right, args);
            const QByteArray fullTemplate = nameParts.last() + '<' + join(args, ", ") + '>';
            if (m_inArgList) // we only want the template grouping on top-level
                nameParts.removeLast();
            nameParts.push_back(fullTemplate);
            break;
        }
        case DEMANGLE_COMPONENT_TEMPLATE_PARAM:
            nameParts.push_back(m_templateParams.value(component->u.s_number.number));
            break;
        case DEMANGLE_COMPONENT_FUNCTION_PARAM:
            // no idea what this means, but that's what c++filt is outputting for these...
            nameParts.push_back(QByteArray("{parm#") + QByteArray::number((int)component->u.s_number.number) + '}');
            break;
        case DEMANGLE_COMPONENT_CTOR:
            // TODO: do we need to consider u.s_ctor.kind?
            handleNameComponent(component->u.s_ctor.name, nameParts);
            break;
        case DEMANGLE_COMPONENT_DTOR:
            // TODO: do we need to consider u.s_dtor.kind?
            handleNameComponent(component->u.s_dtor.name, nameParts);
            nameParts.last().prepend('~');
            break;
        case DEMANGLE_COMPONENT_VTABLE:
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.push_back("vtable");
            break;
        case DEMANGLE_COMPONENT_VTT:
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.push_back("vtt"); // what's this?
            break;
        case DEMANGLE_COMPONENT_CONSTRUCTION_VTABLE:
        {
            handleNameComponent(component->u.s_binary.left, nameParts);
            QVector<QByteArray> tmp;
            handleNameComponent(component->u.s_binary.right, tmp);
            nameParts.push_back("construction vtable in " + join(tmp, "::"));
            break;
        }
        case DEMANGLE_COMPONENT_TYPEINFO:
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.push_back("typeinfo");
            break;
        case DEMANGLE_COMPONENT_TYPEINFO_NAME:
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.push_back("typeinfo name");
            break;
        case DEMANGLE_COMPONENT_TYPEINFO_FN:
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.push_back("typeinfo function");
            break;
        case DEMANGLE_COMPONENT_THUNK:
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.push_back("thunk");
            break;
        case DEMANGLE_COMPONENT_VIRTUAL_THUNK:
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.push_back("virtual thunk");
            break;
        case DEMANGLE_COMPONENT_COVARIANT_THUNK:
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.push_back("covariant return thunk");
            break;
        case DEMANGLE_COMPONENT_GUARD:
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.push_back("guard variable");
            break;
        case DEMANGLE_COMPONENT_REFTEMP:
        {
            handleNameComponent(component->u.s_binary.left, nameParts);
            QVector<QByteArray> tmp;
            handleNameComponent(component->u.s_binary.right, tmp);
            nameParts.push_back("reference temporary #" + tmp.last());
            break;
        }
        case DEMANGLE_COMPONENT_HIDDEN_ALIAS:
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.push_back("hidden alias");
            break;
        case DEMANGLE_COMPONENT_SUB_STD:
            nameParts.push_back(QByteArray(component->u.s_name.s, component->u.s_name.len));
            break;
        case DEMANGLE_COMPONENT_RESTRICT:
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.last().append(" restrict");
            break;
        case DEMANGLE_COMPONENT_VOLATILE:
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.last().append(" volatile");
            break;
        case DEMANGLE_COMPONENT_CONST:
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.last().append(" const");
            break;
        case DEMANGLE_COMPONENT_RESTRICT_THIS:
            handleNameComponent(component->u.s_binary.left, nameParts);
            m_modifiers.append(" restrict");
            break;
        case DEMANGLE_COMPONENT_VOLATILE_THIS:
            handleNameComponent(component->u.s_binary.left, nameParts);
            m_modifiers.append(" volatile");
            break;
        case DEMANGLE_COMPONENT_CONST_THIS:
            handleNameComponent(component->u.s_binary.left, nameParts);
            m_modifiers.append(" const");
            break;
#if BINUTILS_VERSION >= BINUTILS_VERSION_CHECK(2, 24)
        case DEMANGLE_COMPONENT_REFERENCE_THIS:
            handleNameComponent(component->u.s_binary.left, nameParts);
            m_modifiers.append(" &");
            break;
        case DEMANGLE_COMPONENT_RVALUE_REFERENCE_THIS:
            handleNameComponent(component->u.s_binary.left, nameParts);
            m_modifiers.append(" &&");
            break;
#endif
        case DEMANGLE_COMPONENT_VENDOR_TYPE_QUAL:
        {
            QVector<QByteArray> parts;
            handleNameComponent(component->u.s_binary.left, parts);
            handleNameComponent(component->u.s_binary.right, parts);
            nameParts.push_back(parts.first() + ' ' + parts.last());
            break;
        }
        case DEMANGLE_COMPONENT_POINTER:
        {
            QScopedValueRollback<bool> resetter(m_pendingPointer, true);
            handleNameComponent(component->u.s_binary.left, nameParts);
            if (m_pendingPointer) // not consumed by a function pointer
                nameParts.last().append('*');
            break;
        }
        case DEMANGLE_COMPONENT_REFERENCE:
        {
            QScopedValueRollback<bool> resetter(m_pendingReference, true);
            handleNameComponent(component->u.s_binary.left, nameParts);
            if (m_pendingReference && !nameParts.last().endsWith('&')) // not consumed by the array type, and primitive reference collapsing
                nameParts.last().append('&');
            break;
        }
        case DEMANGLE_COMPONENT_RVALUE_REFERENCE:
            handleNameComponent(component->u.s_binary.left, nameParts);
            // skip appending &&
            // - in case of reference collapsing (TODO: this should be done on the tree, not the string)
            // - if we have an empty template arg in a pack expansion
            if (!nameParts.last().endsWith('&') && !nameParts.last().isEmpty())
                nameParts.last().append("&&");
            break;
        case DEMANGLE_COMPONENT_BUILTIN_TYPE:
            nameParts.push_back(QByteArray(component->u.s_builtin.type->name, component->u.s_builtin.type->len));
            break;
        case DEMANGLE_COMPONENT_FUNCTION_TYPE:
        {
            const bool previousPendingPointer = m_pendingPointer;
            m_pendingPointer = false;

            // left is return type (only relevant in argument lists), right is the (optional) argument list
            QVector<QByteArray> returnType;
            handleOptionalNameComponent(component->u.s_binary.left, returnType);

            QVector<QByteArray> args;
            handleOptionalNameComponent(component->u.s_binary.right, args);
            QByteArray fullName;
            if (m_inArgList && !returnType.isEmpty())
                fullName.prepend(returnType.last() + ' ');
            if (previousPendingPointer) { // function pointer
                fullName.append("(*)");
            } else if (!m_ptrmemType.isEmpty()) {
                fullName.append('(' + m_ptrmemType + "::*)");
            } else {
                m_pendingPointer = previousPendingPointer;
            }
            fullName.append('(' + join(args, ", ") + ')');
            nameParts.push_back(fullName);
            break;
        }
        case DEMANGLE_COMPONENT_ARRAY_TYPE:
        {
            const bool prevRef = m_pendingReference;
            m_pendingReference = false;
            // left is optional dimension, right is type
            handleNameComponent(component->u.s_binary.right, nameParts);
            QVector<QByteArray> dim;
            handleOptionalNameComponent(component->u.s_binary.left, dim);
            QByteArray suffix;
            if (prevRef) {
                suffix += " (&)"; // array references are special...
            } else {
                m_pendingReference = prevRef;
            }
            suffix += " [";
            if (!dim.isEmpty())
                suffix.append(dim.last());
            suffix += ']';
            nameParts.last().append(suffix);
            break;
        }
        case DEMANGLE_COMPONENT_PTRMEM_TYPE:
        {
            QScopedValueRollback<QByteArray> ptrmemTypeResetter(m_ptrmemType);
            m_ptrmemType.clear();
            QVector<QByteArray> tmp;
            handleNameComponent(component->u.s_binary.left, tmp);
            m_ptrmemType = tmp.last();
            handleNameComponent(component->u.s_binary.right, nameParts);
            break;
        }
        case DEMANGLE_COMPONENT_VECTOR_TYPE:
        {
            QVector<QByteArray> parts;
            // left is size, right is type
            handleNameComponent(component->u.s_binary.left, parts);
            handleNameComponent(component->u.s_binary.right, parts);
            nameParts.push_back(parts.last() + " __vector(" + parts.first() + ')');
            break;
        }
        case DEMANGLE_COMPONENT_ARGLIST:
        {
            QScopedValueRollback<bool> resetter(m_inArgList, true);
            if (!component->u.s_binary.left && !component->u.s_binary.right) {
                nameParts.push_back(QByteArray("")); // empty arg list
            } else {
                handleOptionalNameComponent(component->u.s_binary.left, nameParts);
                handleOptionalNameComponent(component->u.s_binary.right, nameParts);
            }
            break;
        }
        case DEMANGLE_COMPONENT_TEMPLATE_ARGLIST:
        {
            QScopedValueRollback<bool> resetter(m_inArgList, true);

            if (component->u.s_binary.left) {
                int currentIndex = -1;
                if (m_indexTemplateArgs)
                    currentIndex = m_templateParamIndex++;
                QVector<QByteArray> left;
                {
                    QScopedValueRollback<bool> resetter(m_indexTemplateArgs, false);
                    handleNameComponent(component->u.s_binary.left, left);
                }
                nameParts += left;
                if (m_indexTemplateArgs) {
                    Q_ASSERT(currentIndex >= 0);
                    if (left.isEmpty())
                        m_templateParams.insert(currentIndex, QByteArray()); // empty template arg, might be referenced from elsewhere...
                    else
                        m_templateParams.insert(currentIndex, nameParts.last());
                }
            }

            handleOptionalNameComponent(component->u.s_binary.right, nameParts);
            break;
        }
#if BINUTILS_VERSION >= BINUTILS_VERSION_CHECK(2, 32)
        case DEMANGLE_COMPONENT_TPARM_OBJ:
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.last().prepend(QByteArray("template parameter object for "));
            break;
#endif
#if BINUTILS_VERSION >= BINUTILS_VERSION_CHECK(2, 23)
        case DEMANGLE_COMPONENT_INITIALIZER_LIST:
	{
            QVector<QByteArray> parts;
	    handleNameComponent(component->u.s_binary.left, parts);
            handleNameComponent(component->u.s_binary.right, parts);
	    nameParts.push_back(parts.at(0) + "{" + parts.at(1) + "}");
	    break;
	}
#endif
        case DEMANGLE_COMPONENT_OPERATOR:
            nameParts.push_back(QByteArray("operator") + QByteArray(component->u.s_operator.op->name, component->u.s_operator.op->len));
            break;
        case DEMANGLE_COMPONENT_EXTENDED_OPERATOR:
            handleNameComponent(component->u.s_extended_operator.name, nameParts);
            break;
        case DEMANGLE_COMPONENT_CAST:
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.last().prepend("operator ");
            break;
#if BINUTILS_VERSION >= BINUTILS_VERSION_CHECK(2, 26)
        case DEMANGLE_COMPONENT_CONVERSION:
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.last().prepend("operator ");
            break;
#endif
#if BINUTILS_VERSION >= BINUTILS_VERSION_CHECK(2, 23)
        case DEMANGLE_COMPONENT_NULLARY:
            handleNameComponent(component->u.s_binary.left, nameParts);
            break;
#endif
        case DEMANGLE_COMPONENT_UNARY:
        {
            handleOperatorComponent(component->u.s_binary.left, nameParts);
            handleNameComponent(component->u.s_binary.right, nameParts);
            const QByteArray arg = nameParts.takeLast();
            const QByteArray op = nameParts.takeLast();
            nameParts.push_back(op + arg);
            break;
        }
        case DEMANGLE_COMPONENT_BINARY:
        {
            handleOperatorComponent(component->u.s_binary.left, nameParts);
            handleNameComponent(component->u.s_binary.right, nameParts);
            const QByteArray arg2 = nameParts.takeLast();
            const QByteArray arg1 = nameParts.takeLast();
            const QByteArray op = nameParts.takeLast();
            nameParts.push_back(arg1 + op + arg2);
            break;
        }
        case DEMANGLE_COMPONENT_BINARY_ARGS:
        case DEMANGLE_COMPONENT_TRINARY:
        case DEMANGLE_COMPONENT_TRINARY_ARG1:
        case DEMANGLE_COMPONENT_TRINARY_ARG2:
            handleNameComponent(component->u.s_binary.left, nameParts);
            handleNameComponent(component->u.s_binary.right, nameParts);
            break;
        case DEMANGLE_COMPONENT_LITERAL:
        case DEMANGLE_COMPONENT_LITERAL_NEG:
        {
            // left is type, right is value
            QVector<QByteArray> type;
            handleNameComponent(component->u.s_binary.left, type);
            handleNameComponent(component->u.s_binary.right, type);
            if (component->type == DEMANGLE_COMPONENT_LITERAL_NEG)
                type.last().prepend('-');
            QByteArray typeStr;
            // TODO add: unsigned, long, long long, unsigned long long
            if (type.first() == "bool") {
                typeStr = type.last() == "0" ? "false" : "true";
            } else if (type.first() == "int") {
                typeStr = type.last();
            } else if (type.first() == "unsigned long") {
                typeStr = type.last() + "ul";
            } else { // custom type
                typeStr = '(' + type.first() + ')' + type.last();
            }
            nameParts.push_back(typeStr);
            break;
        }
        case DEMANGLE_COMPONENT_NUMBER:
            nameParts.push_back(QByteArray::number((int)component->u.s_number.number));
            break;
        case DEMANGLE_COMPONENT_DECLTYPE:
            // TODO: undocumented, but one seems to contain content at least
            handleOptionalNameComponent(component->u.s_binary.left, nameParts);
            handleOptionalNameComponent(component->u.s_binary.right, nameParts);
            break;
        case DEMANGLE_COMPONENT_LAMBDA:
        {
            QVector<QByteArray> args;
            handleNameComponent(component->u.s_unary_num.sub, args);
            nameParts.push_back("{lambda(" + join(args, ", ") + ")#" + QByteArray::number(component->u.s_unary_num.num + 1) + '}');
            break;
        }
        case DEMANGLE_COMPONENT_DEFAULT_ARG:
            nameParts.push_back(QByteArray("{default arg#") + QByteArray::number((int)component->u.s_unary_num.num + 1) + '}');
            handleOptionalNameComponent(component->u.s_unary_num.sub, nameParts);
            break;
        case DEMANGLE_COMPONENT_UNNAMED_TYPE:
            nameParts.push_back(QByteArray("{unnamed type#") + QByteArray::number((int)component->u.s_number.number + 1) + '}');
            break;
#if BINUTILS_VERSION >= BINUTILS_VERSION_CHECK(2, 23)
        case DEMANGLE_COMPONENT_TRANSACTION_CLONE:
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.last().prepend("transaction clone for ");
            break;
        case DEMANGLE_COMPONENT_NONTRANSACTION_CLONE:
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.last().prepend("non-transaction clone for ");
            break;
#endif
        case DEMANGLE_COMPONENT_PACK_EXPANSION:
            handleOptionalNameComponent(component->u.s_binary.left, nameParts);
            handleOptionalNameComponent(component->u.s_binary.right, nameParts);
            break;
#if BINUTILS_VERSION >= BINUTILS_VERSION_CHECK(2, 24)
        case DEMANGLE_COMPONENT_TAGGED_NAME:
        {
            QVector<QByteArray> args;
            handleNameComponent(component->u.s_binary.left, nameParts);
            handleNameComponent(component->u.s_binary.right, args);
            const auto n = nameParts.takeLast();
            nameParts.push_back(n + "[abi:" + args.last() + ']');
            break;
        }
#endif
#if BINUTILS_VERSION >= BINUTILS_VERSION_CHECK(2, 23)
        case DEMANGLE_COMPONENT_CLONE:
        {
            QVector<QByteArray> args;
            handleNameComponent(component->u.s_binary.left, nameParts);
            handleNameComponent(component->u.s_binary.right, args);
            const auto n = nameParts.takeLast();
            nameParts.push_back(n + " [clone " + args.last() + ']');
            break;
        }
#endif
        default:
            qDebug() << Q_FUNC_INFO << "unhandled component type" << component->type << m_mangledName;
    }
}

void Demangler::handleOptionalNameComponent(demangle_component* component, QVector< QByteArray >& nameParts)
{
    if (!component)
        return;
    handleNameComponent(component, nameParts);
}

void Demangler::handleOperatorComponent(demangle_component* component, QVector< QByteArray >& nameParts)
{
    if (component->type == DEMANGLE_COMPONENT_OPERATOR) {
        nameParts.push_back(QByteArray(component->u.s_operator.op->name, component->u.s_operator.op->len));
        return;
    }
    handleNameComponent(component, nameParts);
}

Demangler::SymbolType Demangler::symbolType(const char* name)
{
    if (strlen(name) < 4)
        return SymbolType::Normal;
    if (name[0] != '_' || name[1] != 'Z' || name[2] != 'T')
        return SymbolType::Normal;

    switch (name[3]) {
        case 'V': return SymbolType::VTable;
        case 'I': return SymbolType::TypeInfo;
        case 'S': return SymbolType::TypeInfoName;
        case 'T': return SymbolType::VTT;
        case 'C': return SymbolType::ConstructionVTable;
    }

    return SymbolType::Normal;
}
