/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DWARFTYPES_H
#define DWARFTYPES_H

#include <QMetaType>

#include <dwarf.h>

enum class DwarfVirtuality : uint8_t
{
    None = DW_VIRTUALITY_none,
    Virtual = DW_VIRTUALITY_virtual,
    PureVirtual = DW_VIRTUALITY_pure_virtual
};

Q_DECLARE_METATYPE(DwarfVirtuality)

#endif
