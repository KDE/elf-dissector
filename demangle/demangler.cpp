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

    qDebug() << name << component << memory << component->type;
    size_t size;
    char * fullName = cplus_demangle_print(DMGL_NO_OPTS, component, strlen(name), &size);
    result << fullName;
    free(fullName);

    handleNameComponent(component, result);

    free(memory);
    qDebug() << result;
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
        case DEMANGLE_COMPONENT_TEMPLATE:
            handleNameComponent(component->u.s_binary.left, nameParts);
            // TODO handle template args?
            break;
        case DEMANGLE_COMPONENT_CTOR:
            // TODO: do we need to consider u.s_ctor.kind?
            handleNameComponent(component->u.s_ctor.name, nameParts);
            break;
        case DEMANGLE_COMPONENT_VTABLE:
            nameParts.push_back("vtable");
            break;
        case DEMANGLE_COMPONENT_DTOR:
            // TODO: do we need to consider u.s_dtor.kind?
            handleNameComponent(component->u.s_dtor.name, nameParts);
            break;
        case DEMANGLE_COMPONENT_OPERATOR:
            nameParts.push_back(QByteArray("operator") + QByteArray(component->u.s_operator.op->name, component->u.s_operator.op->len));
            break;
        case DEMANGLE_COMPONENT_EXTENDED_OPERATOR:
            handleNameComponent(component->u.s_extended_operator.name, nameParts);
            break;
        default:
            qDebug() << Q_FUNC_INFO << "unhandled component type" << component->type;
    }
}
