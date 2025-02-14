/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DEMANGLER_H
#define DEMANGLER_H

#include <QByteArray>
#include <QHash>
#include <QMetaType>
#include <QList>

struct demangle_component;

/** C++ name demangler. */
class Demangler
{
public:
    Demangler() = default;
    Demangler(const Demangler &other) = delete;
    Demangler& operator=(const Demangler &other) = delete;

    /** Demangle the given name and return the name split in namespace(s)/class/method. */
    QList<QByteArray> demangle(const char* name);

    /** Demangle the given name into a single string. */
    static QByteArray demangleFull(const char* name);

    enum class SymbolType {
        Normal,
        VTable,
        TypeInfo,
        TypeInfoName,
        VTT,
        ConstructionVTable
    };
    /** Determine type of C++ symbols. */
    static SymbolType symbolType(const char* name);

private:
    void reset();
    void handleNameComponent(demangle_component *component, QList<QByteArray> &nameParts);
    void handleOptionalNameComponent(demangle_component *component, QList<QByteArray> &nameParts);
    void handleOperatorComponent(demangle_component *component, QList<QByteArray> &nameParts);

    const char *m_mangledName = nullptr;
    int m_templateParamIndex = 0;
    QHash<int, QByteArray> m_templateParams;
    QByteArray m_modifiers;
    QByteArray m_ptrmemType;
    bool m_inArgList = false;
    bool m_pendingPointer = false;
    bool m_pendingReference = false;
    bool m_indexTemplateArgs = false;

    // TODO shared value caching
};

Q_DECLARE_METATYPE(Demangler::SymbolType)

#endif // DEMANGLER_H
