#ifndef DEMANGLER_H
#define DEMANGLER_H

#include <QByteArray>
#include <QVector>

/** C++ name demangler. */
class Demangler
{
public:
    Demangler() = default;
    Demangler(const Demangler &other) = delete;
    Demangler& operator=(const Demangler &other) = delete;

    /** Demange the given name and return the name splitted in namespace(s)/class/method. */
    QVector<QByteArray> demangle(const char* name) const;

private:
    // TODO shared value caching
};

#endif // DEMANGLER_H
