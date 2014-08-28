#include "colorizer.h"

#include <QColor>
#include <QDebug>

QColor Colorizer::nextColor()
{
    QColor c = QColor::fromHsv(m_hue, 255, 192);
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
