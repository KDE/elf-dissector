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

// workarounds for conflicting declaration in libiberty.h
#define HAVE_DECL_BASENAME 1
#define HAVE_DECL_ASPRINTF 1
#define HAVE_DECL_VASPRINTF 1

#include <demangle.h>

#include <QByteArray>

#include <iostream>

using namespace std;

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

static int nodeCount = 0;

static void writeNode(const char* name)
{
    cout << "    node" << nodeCount << "[label = \"" << name << "\"];" << endl;
}

static void writeLink(int source, int target)
{
    if (source == 0 || target == 0)
        return;
    cout << "    node" << source << " -> node" << target << ";" << endl;
}

static int handleNameComponent(demangle_component* component)
{
    if (!component)
        return 0;

    ++nodeCount;
    const int sourceNode = nodeCount;
    switch (component->type) {
        case DEMANGLE_COMPONENT_NAME:
            writeNode(QByteArray(component->u.s_name.s, component->u.s_name.len));
            break;
        case DEMANGLE_COMPONENT_QUAL_NAME:
            writeNode("QUAL_NAME");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
        case DEMANGLE_COMPONENT_LOCAL_NAME:
            writeNode("LOCAL_NAME");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
        case DEMANGLE_COMPONENT_TYPED_NAME:
            writeNode("TYPED_NAME");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
        case DEMANGLE_COMPONENT_TEMPLATE:
            writeNode("TEMPLATE");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
        case DEMANGLE_COMPONENT_TEMPLATE_PARAM:
            writeNode(QByteArray("TEMPLATE_PARAM: ") + QByteArray::number((int)component->u.s_number.number));
            // ???
            break;
        case DEMANGLE_COMPONENT_FUNCTION_PARAM:
            writeNode(QByteArray("FUNCTION_PARAM: ") + QByteArray::number((int)component->u.s_number.number));
            // ??? "This holds a number, which is the  parameter index."
            break;
        case DEMANGLE_COMPONENT_CTOR:
            writeNode("CTOR");
            // TODO: do we need to consider u.s_ctor.kind?
            writeLink(sourceNode, handleNameComponent(component->u.s_ctor.name));
            break;
        case DEMANGLE_COMPONENT_DTOR:
            writeNode("DTOR");
            // TODO: do we need to consider u.s_dtor.kind?
            writeLink(sourceNode, handleNameComponent(component->u.s_dtor.name));
            break;
        case DEMANGLE_COMPONENT_VTABLE:
            writeNode("VTABLE");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
        case DEMANGLE_COMPONENT_VTT:
            writeNode("VTT");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
        case DEMANGLE_COMPONENT_CONSTRUCTION_VTABLE:
            writeNode("CONSTRUCTION_VTABLE");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
        case DEMANGLE_COMPONENT_TYPEINFO:
            writeNode("TYPEINFO");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
        case DEMANGLE_COMPONENT_TYPEINFO_NAME:
            writeNode("TYPEINFO_NAME");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
        case DEMANGLE_COMPONENT_TYPEINFO_FN:
            writeNode("TYPEINFO_FN");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
        case DEMANGLE_COMPONENT_THUNK:
            writeNode("THUNK");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
        case DEMANGLE_COMPONENT_VIRTUAL_THUNK:
            writeNode("VIRTUAL_THUNK");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
        case DEMANGLE_COMPONENT_COVARIANT_THUNK:
            writeNode("COVARIANT_THUNK");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
        case DEMANGLE_COMPONENT_JAVA_CLASS:
            writeNode("JAVA_CLASS");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
        case DEMANGLE_COMPONENT_GUARD:
            writeNode("GUARD");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
#if BINUTILS_VERSION >= BINUTILS_VERSION_CHECK(2, 24)
        case DEMANGLE_COMPONENT_TLS_INIT:
            writeNode("TLS_INIT");
            // content?
            break;
        case DEMANGLE_COMPONENT_TLS_WRAPPER:
            writeNode("TLS_WRAPPER");
            // content ??
            break;
#endif
        case DEMANGLE_COMPONENT_REFTEMP:
            writeNode("REFTEMP");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
        case DEMANGLE_COMPONENT_HIDDEN_ALIAS:
            writeNode("HIDDEN_ALIAS");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
        case DEMANGLE_COMPONENT_SUB_STD:
            writeNode(QByteArray("SUB_STD: ") + QByteArray(component->u.s_name.s, component->u.s_name.len));
            break;
        case DEMANGLE_COMPONENT_RESTRICT:
            writeNode("RESTRICT");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
        case DEMANGLE_COMPONENT_VOLATILE:
            writeNode("VOLATILE");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
        case DEMANGLE_COMPONENT_CONST:
            writeNode("CONST");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
        case DEMANGLE_COMPONENT_RESTRICT_THIS:
            writeNode("RESTRICT_THIS");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
        case DEMANGLE_COMPONENT_VOLATILE_THIS:
            writeNode("VOLATILE_THIS");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
        case DEMANGLE_COMPONENT_CONST_THIS:
            writeNode("CONST_THIS");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
#if BINUTILS_VERSION >= BINUTILS_VERSION_CHECK(2, 24)
        case DEMANGLE_COMPONENT_REFERENCE_THIS:
            writeNode("REFERENCE_THIS");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
        case DEMANGLE_COMPONENT_RVALUE_REFERENCE_THIS:
            writeNode("RVALUE_REFERENCE_THIS");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
#endif
        case DEMANGLE_COMPONENT_VENDOR_TYPE_QUAL:
            writeNode("VENDOR_TYPE_QUAL");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
        case DEMANGLE_COMPONENT_POINTER:
            writeNode("POINTER");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
        case DEMANGLE_COMPONENT_REFERENCE:
            writeNode("REFERENCE");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
        case DEMANGLE_COMPONENT_RVALUE_REFERENCE:
            writeNode("RVALUE_REFERENCE");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
        case DEMANGLE_COMPONENT_COMPLEX:
            writeNode("COMPLEX");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
        case DEMANGLE_COMPONENT_IMAGINARY:
            writeNode("IMAGINARY");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
        case DEMANGLE_COMPONENT_BUILTIN_TYPE:
            writeNode(QByteArray("BUILTIN_TYPE: ") + QByteArray(component->u.s_builtin.type->name, component->u.s_builtin.type->len));
            break;
        case DEMANGLE_COMPONENT_VENDOR_TYPE:
            writeNode("VENDOR_TYPE");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
        case DEMANGLE_COMPONENT_FUNCTION_TYPE:
            writeNode("FUNCTION_TYPE");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
        case DEMANGLE_COMPONENT_ARRAY_TYPE:
            writeNode("ARRAY_TYPE");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
        case DEMANGLE_COMPONENT_PTRMEM_TYPE:
            writeNode("PTRMEM_TYPE");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
        case DEMANGLE_COMPONENT_FIXED_TYPE:
            writeNode("FIXED_TYPE");
            // TODO ???
            break;
        case DEMANGLE_COMPONENT_VECTOR_TYPE:
            writeNode("VECTOR_TYPE");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
        case DEMANGLE_COMPONENT_ARGLIST:
            writeNode("ARGLIST");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
        case DEMANGLE_COMPONENT_TEMPLATE_ARGLIST:
            writeNode("TEMPLATE_ARGLIST");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
#if BINUTILS_VERSION >= BINUTILS_VERSION_CHECK(2, 32)
        case DEMANGLE_COMPONENT_TPARM_OBJ:
            writeNode("TPARM_OBJ");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
	    break;
#endif
#if BINUTILS_VERSION >= BINUTILS_VERSION_CHECK(2, 23)
        case DEMANGLE_COMPONENT_INITIALIZER_LIST:
            writeNode("INITIALIZER_LIST");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
#endif
        case DEMANGLE_COMPONENT_OPERATOR:
            writeNode(QByteArray("OPERATOR ") + QByteArray(component->u.s_operator.op->name, component->u.s_operator.op->len));
            break;
        case DEMANGLE_COMPONENT_EXTENDED_OPERATOR:
            writeNode("EXTENDED_OPERATOR");
            // TODO
  /* An extended operator.  This holds the number of arguments, and
     the name of the extended operator.  */
            break;
        case DEMANGLE_COMPONENT_CAST:
            writeNode("CAST");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
#if BINUTILS_VERSION >= BINUTILS_VERSION_CHECK(2, 26)
        case DEMANGLE_COMPONENT_CONVERSION:
            writeNode("CONVERSION");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
#endif
#if BINUTILS_VERSION >= BINUTILS_VERSION_CHECK(2, 23)
        case DEMANGLE_COMPONENT_NULLARY:
            writeNode("NULLARY");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
#endif
        case DEMANGLE_COMPONENT_UNARY:
            writeNode("UNARY");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
        case DEMANGLE_COMPONENT_BINARY:
            writeNode("BINARY");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
        case DEMANGLE_COMPONENT_BINARY_ARGS:
            writeNode("BINARY_ARGS");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
        case DEMANGLE_COMPONENT_TRINARY:
            writeNode("TRINARY");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
        case DEMANGLE_COMPONENT_TRINARY_ARG1:
            writeNode("TRINARY_ARG1");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
        case DEMANGLE_COMPONENT_TRINARY_ARG2:
            writeNode("TRINARY_ARG2");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
        case DEMANGLE_COMPONENT_LITERAL:
            writeNode("LITERAL");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
        case DEMANGLE_COMPONENT_LITERAL_NEG:
            writeNode("LITERAL_NEG");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
        case DEMANGLE_COMPONENT_JAVA_RESOURCE:
            writeNode("JAVA_RESOURCE");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
        case DEMANGLE_COMPONENT_COMPOUND_NAME:
            writeNode("COMPOUND_NAME");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
        case DEMANGLE_COMPONENT_CHARACTER:
            writeNode("CHARACTER");
            // TODO: content?
            break;
        case DEMANGLE_COMPONENT_NUMBER:
            writeNode(QByteArray("NUMBER: ") + QByteArray::number((int)component->u.s_number.number));
            break;
        case DEMANGLE_COMPONENT_DECLTYPE:
            writeNode("DECLTYPE");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            // TODO: content?
            break;
        case DEMANGLE_COMPONENT_GLOBAL_CONSTRUCTORS:
            writeNode("GLOBAL_CONSTRUCTORS");
            // TODO: content?
            break;
        case DEMANGLE_COMPONENT_GLOBAL_DESTRUCTORS:
            writeNode("GLOBAL_DESTRUCTORS");
            // TODO: content?
            break;
        case DEMANGLE_COMPONENT_LAMBDA:
            writeNode(QByteArray("LAMBDA: ") + QByteArray::number(component->u.s_unary_num.num));
            writeLink(sourceNode, handleNameComponent(component->u.s_unary_num.sub));
            break;
        case DEMANGLE_COMPONENT_DEFAULT_ARG:
            writeNode(QByteArray("DEFAULT_ARG: ") + QByteArray::number((int)component->u.s_unary_num.num));
            writeLink(sourceNode, handleNameComponent(component->u.s_unary_num.sub));
            break;
        case DEMANGLE_COMPONENT_UNNAMED_TYPE:
            writeNode(QByteArray("UNNAMED_TYPE: ") + QByteArray::number((int)component->u.s_number.number));
            break;
#if BINUTILS_VERSION >= BINUTILS_VERSION_CHECK(2, 23)
        case DEMANGLE_COMPONENT_TRANSACTION_CLONE:
            writeNode("TRANSACTION_CLONE");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
        case DEMANGLE_COMPONENT_NONTRANSACTION_CLONE:
            writeNode("NONTRANSACTION_CLONE");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            break;
#endif
        case DEMANGLE_COMPONENT_PACK_EXPANSION:
            writeNode("PACK_EXPANSION");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
#if BINUTILS_VERSION >= BINUTILS_VERSION_CHECK(2, 24)
        case DEMANGLE_COMPONENT_TAGGED_NAME:
            writeNode("TAGGED_NAME");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
#endif
#if BINUTILS_VERSION >= BINUTILS_VERSION_CHECK(2, 27)
        case DEMANGLE_COMPONENT_TRANSACTION_SAFE:
            writeNode("TRANSACTION_SAFE");
            // ??
            break;
#endif
#if BINUTILS_VERSION >= BINUTILS_VERSION_CHECK(2, 23)
        case DEMANGLE_COMPONENT_CLONE:
            writeNode("COMPONENT_CLONE");
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.left));
            writeLink(sourceNode, handleNameComponent(component->u.s_binary.right));
            break;
#endif
#if BINUTILS_VERSION >= BINUTILS_VERSION_CHECK(2, 28)
        case DEMANGLE_COMPONENT_NOEXCEPT:
            writeNode("NOEXCEPT");
            // ??
            break;
        case DEMANGLE_COMPONENT_THROW_SPEC:
            writeNode("THROW_SPEC");
            // ??
            break;
#endif
    }
    return sourceNode;
}

int main(int argc, char** argv)
{
    if (argc < 2)
        return 1;

    void *memory = nullptr;
    demangle_component *component = cplus_demangle_v3_components(argv[1], DMGL_PARAMS | DMGL_ANSI | DMGL_TYPES | DMGL_VERBOSE, &memory);

    cout << "digraph ast {" << endl;

    handleNameComponent(component);

    cout << "}" << endl;

    size_t size;
    char * fullName = cplus_demangle_print(DMGL_PARAMS | DMGL_ANSI | DMGL_TYPES | DMGL_VERBOSE, component, strlen(argv[1]), &size);
    cerr << fullName << endl;
    free(fullName);

    free(memory);
    return 0;
}
