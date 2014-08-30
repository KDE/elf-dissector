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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
    QColor nextColor();

private:
    uint16_t m_hue = 0;
    uint8_t m_increment = 180;
    uint8_t m_offset = 90;
    uint16_t m_count = 0;
};

#endif // COLORIZER_H
