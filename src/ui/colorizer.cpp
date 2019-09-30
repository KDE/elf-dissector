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
