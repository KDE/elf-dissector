/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "colorizer.h"

#include <QColor>


Colorizer::Colorizer(uint8_t saturation, uint8_t value):
    m_saturation(saturation),
    m_value(value)
{
}

QColor Colorizer::nextColor()
{
    QColor c = QColor::fromHsv(m_hue, m_saturation, m_value);
    ++m_count;

    if (m_count * m_increment >= 360) {
        m_count = 0;
        m_hue += m_offset;
        if ((m_offset * 2) < m_increment && m_increment > 1)
            m_increment /= 2;
        m_offset /= 2;
    }

    m_hue += m_increment;
    m_hue = m_hue % 360;

    return c;
}
