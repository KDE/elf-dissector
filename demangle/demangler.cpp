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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "demangler.h"

#include <QDebug>

// workarounds for conflicting declaration in libiberty.h
#define HAVE_DECL_BASENAME 1
#define HAVE_DECL_ASPRINTF 1
#define HAVE_DECL_VASPRINTF 1

#include <demangle.h>


QVector<QByteArray> Demangler::demangle(const char* name)
{
    void *memory = 0;
    demangle_component *component = cplus_demangle_v3_components(name, DMGL_NO_OPTS, &memory);

    QVector<QByteArray> result;
    if (!memory || !component) { // demange failed, likely not mangled
        result.push_back(name);
        return result;
    }

    handleNameComponent(component, result);
    if (!result.isEmpty()) {
        free(memory);
        return result;
    }

    qDebug() << name << component << memory << component->type;
    size_t size;
    char * fullName = cplus_demangle_print(DMGL_NO_OPTS, component, strlen(name), &size);
    result << fullName;
    free(fullName);

    free(memory);
    return result;
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
            break;
        case DEMANGLE_COMPONENT_LOCAL_NAME:
            handleNameComponent(component->u.s_binary.left, nameParts);
            handleNameComponent(component->u.s_binary.right, nameParts);
            break;
        case DEMANGLE_COMPONENT_TYPED_NAME:
            handleNameComponent(component->u.s_binary.left, nameParts);
            // TODO: right side relevant?
            break;
        case DEMANGLE_COMPONENT_TEMPLATE:
            handleNameComponent(component->u.s_binary.left, nameParts);
            // TODO handle template args?
            break;
        case DEMANGLE_COMPONENT_TEMPLATE_PARAM:
        case DEMANGLE_COMPONENT_FUNCTION_PARAM:
            // ??? "This holds a number, which is the  parameter index."
            break;
        case DEMANGLE_COMPONENT_CTOR:
            // TODO: do we need to consider u.s_ctor.kind?
            handleNameComponent(component->u.s_ctor.name, nameParts);
            break;
        case DEMANGLE_COMPONENT_DTOR:
            // TODO: do we need to consider u.s_dtor.kind?
            handleNameComponent(component->u.s_dtor.name, nameParts);
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
            nameParts.push_back("construction vtable for");
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.push_back("in");
            handleNameComponent(component->u.s_binary.right, nameParts);
            break;
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
            nameParts.push_back("covariant thunk");
            break;
        case DEMANGLE_COMPONENT_GUARD:
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.push_back("guard");
            break;
        case DEMANGLE_COMPONENT_REFTEMP:
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.push_back("reference temporary");
            break;
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
            nameParts.last().append(" restrict");
            break;
        case DEMANGLE_COMPONENT_VOLATILE_THIS:
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.last().append(" volatile");
            break;
        case DEMANGLE_COMPONENT_CONST_THIS:
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.last().append(" const");
            break;
        case DEMANGLE_COMPONENT_POINTER:
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.last().append("*");
            break;
        case DEMANGLE_COMPONENT_REFERENCE:
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.last().append("&");
            break;
        case DEMANGLE_COMPONENT_RVALUE_REFERENCE:
            handleNameComponent(component->u.s_binary.left, nameParts);
            nameParts.last().append("&&");
            break;
        case DEMANGLE_COMPONENT_BUILTIN_TYPE:
            nameParts.push_back(QByteArray(component->u.s_builtin.type->name, component->u.s_builtin.type->len));
            break;
        case DEMANGLE_COMPONENT_PTRMEM_TYPE:
            handleNameComponent(component->u.s_binary.left, nameParts);
            handleNameComponent(component->u.s_binary.right, nameParts);
            break;
        case DEMANGLE_COMPONENT_OPERATOR:
            nameParts.push_back(QByteArray("operator") + QByteArray(component->u.s_operator.op->name, component->u.s_operator.op->len));
            break;
        case DEMANGLE_COMPONENT_EXTENDED_OPERATOR:
            handleNameComponent(component->u.s_extended_operator.name, nameParts);
            break;
        case DEMANGLE_COMPONENT_CAST:
            // TODO we probably want to mention this is a cast?
            handleNameComponent(component->u.s_binary.left, nameParts);
            break;
        case DEMANGLE_COMPONENT_LAMBDA:
            // TODO what's in here??
            nameParts.push_back("lambda");
            break;
        case DEMANGLE_COMPONENT_UNNAMED_TYPE:
            // TODO what's in here?
            nameParts.push_back("unnamed");
            break;
        default:
            qDebug() << Q_FUNC_INFO << "unhandled component type" << component->type;
    }
}
