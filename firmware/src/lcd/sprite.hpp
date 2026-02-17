#pragma once
#include <cstdint>

namespace lcd {

struct Sprite {
    uint16_t width;
    uint16_t height;
    const uint8_t* pixels;  // RGB888 row-major, 3 bytes per pixel
    const uint8_t* alpha;   // 1-bit packed mask (MSB=leftmost), nullptr if fully opaque
};

} // namespace lcd
