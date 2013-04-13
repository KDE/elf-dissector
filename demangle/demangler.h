#ifndef DEMANGLER_H
#define DEMANGLER_H

#include <QByteArray>
#include <QVector>

struct demangle_component;

/** C++ name demangler. */
class Demangler
{
public:
    Demangler() = default;
    Demangler(const Demangler &other) = delete;
    Demangler& operator=(const Demangler &other) = delete;

    /** Demange the given name and return the name splitted in namespace(s)/class/method. */
    QVector<QByteArray> demangle(const char* name);

private:
    void handleNameComponent(demangle_component *component, QVector<QByteArray> &nameParts);

    // TODO shared value caching
};

#endif // DEMANGLER_H
