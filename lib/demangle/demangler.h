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

#ifndef DEMANGLER_H
#define DEMANGLER_H

#include <QByteArray>
#include <QHash>
#include <QMetaType>
#include <QVector>

struct demangle_component;

/** C++ name demangler. */
class Demangler
{
public:
    Demangler() = default;
    Demangler(const Demangler &other) = delete;
    Demangler& operator=(const Demangler &other) = delete;

    /** Demangle the given name and return the name split in namespace(s)/class/method. */
    QVector<QByteArray> demangle(const char* name);

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
    void handleNameComponent(demangle_component *component, QVector<QByteArray> &nameParts);
    void handleOptionalNameComponent(demangle_component *component, QVector<QByteArray> &nameParts);
    void handleOperatorComponent(demangle_component *component, QVector<QByteArray> &nameParts);

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
