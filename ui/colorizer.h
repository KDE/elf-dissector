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
