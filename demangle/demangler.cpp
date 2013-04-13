#include "demangler.h"

#include <QDebug>

// workarounds for conflicting declaration in libiberty.h
#define HAVE_DECL_BASENAME 1
#define HAVE_DECL_ASPRINTF 1
#define HAVE_DECL_VASPRINTF 1

#include <demangle.h>


QVector<QByteArray> Demangler::demangle(const char* name) const
{
    void *memory = 0;
    demangle_component *component = cplus_demangle_v3_components(name, DMGL_NO_OPTS, &memory);

    QVector<QByteArray> result;
    if (!memory || !component) { // demange failed, likely not mangled
        result.push_back(name);
        return result;
    }

    qDebug() << name << component << memory;
    if (component) {
        size_t size;
        char * fullName = cplus_demangle_print(DMGL_NO_OPTS, component, strlen(name), &size);
        result << fullName;
        qDebug() << fullName;
        free(fullName);
    }

    free(memory);
    return result;
}
