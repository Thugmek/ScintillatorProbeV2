#pragma once
#include <cstdint>

namespace lcd {

inline constexpr uint16_t WIDTH  = 320;
inline constexpr uint16_t HEIGHT = 480;

struct Color {
    uint8_t r, g, b;
};

namespace colors {
    inline constexpr Color BLACK   = {0,   0,   0};
    inline constexpr Color WHITE   = {255, 255, 255};
    inline constexpr Color RED     = {255, 0,   0};
    inline constexpr Color GREEN   = {0,   255, 0};
    inline constexpr Color BLUE    = {0,   0,   255};
    inline constexpr Color YELLOW  = {255, 255, 0};
    inline constexpr Color CYAN    = {0,   255, 255};
    inline constexpr Color MAGENTA = {255, 0,   255};
    inline constexpr Color ORANGE  = {255, 180, 0};
    inline constexpr Color GRAY    = {128, 128, 128};
}

void init();
bool test_connection();
void fill_screen(Color color);
void fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, Color color);
void draw_pixel(uint16_t x, uint16_t y, Color color);
void draw_char(uint16_t x, uint16_t y, char ch, Color fg, Color bg);
void draw_string(uint16_t x, uint16_t y, const char* str, Color fg, Color bg);

struct Sprite;
void draw_sprite(uint16_t x, uint16_t y, const Sprite& sprite, Color bg = colors::BLACK);

} // namespace lcd
