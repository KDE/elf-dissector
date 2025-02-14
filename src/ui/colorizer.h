/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef COLORIZER_H
#define COLORIZER_H

#include <cstdint>

class QColor;

/** Provide visually pleasing and distinct colors for visualization purposes.
 *  Or try at least...
 */
class Colorizer
{
public:
    explicit Colorizer(uint8_t saturation = 255, uint8_t value = 192);
    QColor nextColor();

private:
    uint16_t m_hue = 0;
    uint8_t m_increment = 180;
    uint8_t m_offset = 90;
    uint16_t m_count = 0;
    uint8_t m_saturation;
    uint8_t m_value;
};

#endif // COLORIZER_H
