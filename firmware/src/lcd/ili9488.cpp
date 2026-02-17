#include "lcd/ili9488.hpp"
#include "lcd/sprite.hpp"
#include "lcd/font_8x16.hpp"
#include "hal.hpp"
#include "logging/log.hpp"

LOG_COMPONENT_DEF(LCD, logging::Severity::debug);

namespace lcd {

// ILI9488 command definitions
enum Cmd : uint8_t {
    NOP     = 0x00,
    SWRESET = 0x01,
    SLPOUT  = 0x11,
    INVON   = 0x21,
    DISPON  = 0x29,
    CASET   = 0x2A,
    RASET   = 0x2B,
    RAMWR   = 0x2C,
    MADCTL  = 0x36,
    PIXFMT  = 0x3A,
    PGAMMA  = 0xE0,
    NGAMMA  = 0xE1,
    PWCTR1  = 0xC0,
    PWCTR2  = 0xC1,
    VMCTR   = 0xC5,
    IFMODE  = 0xB0,
    FRMCTR  = 0xB1,
    INVCTR  = 0xB4,
    DISCTRL = 0xB6,
    IMGFUNC = 0xE9,
    ADJCTRL = 0xF7,
};

// Line buffer for pixel data: one row = 320 pixels * 3 frames (R,G,B), each 16-bit
// 3-wire SPI: each byte on the wire is a 9-bit frame (DC bit + 8 data bits),
// stored as uint16_t for the 9-bit HW SPI peripheral.
static uint16_t line_buf[WIDTH * 3];

// --- Pin helpers ---

static inline void cs_low()  { HAL_GPIO_WritePin(LCD_CS_GPIO_Port,  LCD_CS_Pin,  GPIO_PIN_RESET); }
static inline void cs_high() { HAL_GPIO_WritePin(LCD_CS_GPIO_Port,  LCD_CS_Pin,  GPIO_PIN_SET); }
static inline void rst_low() { HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET); }
static inline void rst_high(){ HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET); }

// --- 3-wire HW SPI (9-bit frames) ---
// Bit 8 = DC: 0 = command, 1 = data
// Bits 7:0 = payload byte

static HAL_StatusTypeDef spi_tx9(const uint16_t* data, uint16_t count) {
    return HAL_SPI_Transmit(&hal::hspi1, reinterpret_cast<const uint8_t*>(data), count, HAL_MAX_DELAY);
}

static void write_cmd(uint8_t cmd) {
    uint16_t frame = cmd;  // DC=0
    cs_low();
    spi_tx9(&frame, 1);
    cs_high();
}

static void write_cmd_data(uint8_t cmd, const uint8_t* data, uint16_t len) {
    uint16_t buf[17];  // cmd + up to 16 data bytes
    buf[0] = cmd;  // DC=0
    for (uint16_t i = 0; i < len; i++)
        buf[i + 1] = 0x100 | data[i];  // DC=1
    cs_low();
    spi_tx9(buf, len + 1);
    cs_high();
}

static void write_cmd8(uint8_t cmd, uint8_t val) {
    write_cmd_data(cmd, &val, 1);
}

// Begin streaming pixel data to a rectangular region.
// CS stays asserted — caller must call cs_high() when done.
static void begin_pixel_write(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    { uint8_t d[] = { static_cast<uint8_t>(x0>>8), static_cast<uint8_t>(x0),
                      static_cast<uint8_t>(x1>>8), static_cast<uint8_t>(x1) };
      write_cmd_data(CASET, d, 4); }
    { uint8_t d[] = { static_cast<uint8_t>(y0>>8), static_cast<uint8_t>(y0),
                      static_cast<uint8_t>(y1>>8), static_cast<uint8_t>(y1) };
      write_cmd_data(RASET, d, 4); }
    uint16_t frame = RAMWR;  // DC=0
    cs_low();
    spi_tx9(&frame, 1);
    // CS stays low, subsequent spi_tx9 calls send pixel data (DC=1)
}

// --- HW reset ---

static void hw_reset() {
    rst_high();
    HAL_Delay(5);
    rst_low();
    HAL_Delay(20);
    rst_high();
    HAL_Delay(150);
}

// --- Public API ---

bool test_connection() {
    log_info(LCD, "LCD: 3-wire HW SPI test");
    hw_reset();
    write_cmd(SWRESET);     HAL_Delay(120);
    write_cmd(SLPOUT);      HAL_Delay(120);
    write_cmd8(PIXFMT, 0x66);
    write_cmd8(MADCTL, 0x48);  // MX + BGR
    write_cmd(DISPON);      HAL_Delay(20);

    // Draw 50x50 red square at (0,0)
    begin_pixel_write(0, 0, 49, 49);
    for (int i = 0; i < 50 * 50; i++) {
        uint16_t px[] = { 0x1FF, 0x100, 0x100 };  // R=0xFF, G=0x00, B=0x00
        spi_tx9(px, 3);
    }
    cs_high();

    log_info(LCD, "LCD: test done - red square at (0,0)?");
    return true;
}

void init() {
    hw_reset();
    write_cmd(SWRESET); HAL_Delay(120);
    write_cmd(SLPOUT);  HAL_Delay(120);
    // Gamma correction
    { const uint8_t d[] = {0x00,0x13,0x18,0x04,0x0F,0x06,0x3A,0x56,
                           0x4D,0x03,0x0A,0x06,0x30,0x3E,0x0F};
      write_cmd_data(PGAMMA, d, sizeof(d)); }
    { const uint8_t d[] = {0x00,0x13,0x18,0x01,0x11,0x06,0x38,0x34,
                           0x4D,0x06,0x0D,0x0B,0x31,0x37,0x0F};
      write_cmd_data(NGAMMA, d, sizeof(d)); }
    // Power control
    { const uint8_t d[] = {0x18, 0x16}; write_cmd_data(PWCTR1, d, 2); }
    write_cmd8(PWCTR2, 0x45);
    { const uint8_t d[] = {0x00, 0x63, 0x01}; write_cmd_data(VMCTR, d, 3); }
    // Pixel format and orientation
    write_cmd8(MADCTL, 0x48);   // MX + BGR
    write_cmd8(PIXFMT, 0x66);   // 18-bit RGB666
    write_cmd(DISPON); HAL_Delay(20);
    fill_screen(colors::BLACK);
}

void fill_screen(Color color) { fill_rect(0, 0, WIDTH, HEIGHT, color); }

void fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, Color color) {
    if (x >= WIDTH || y >= HEIGHT || w == 0 || h == 0) return;
    if (x + w > WIDTH)  w = WIDTH - x;
    if (y + h > HEIGHT) h = HEIGHT - y;
    // Pre-fill one row of 9-bit pixel frames (DC=1 | byte)
    for (uint16_t i = 0; i < w; i++) {
        line_buf[i*3+0] = 0x100 | color.r;
        line_buf[i*3+1] = 0x100 | color.g;
        line_buf[i*3+2] = 0x100 | color.b;
    }
    uint16_t row_frames = w * 3;
    begin_pixel_write(x, y, x + w - 1, y + h - 1);
    for (uint16_t row = 0; row < h; row++)
        spi_tx9(line_buf, row_frames);
    cs_high();
}

void draw_pixel(uint16_t x, uint16_t y, Color color) {
    if (x >= WIDTH || y >= HEIGHT) return;
    begin_pixel_write(x, y, x, y);
    uint16_t px[] = { static_cast<uint16_t>(0x100 | color.r),
                      static_cast<uint16_t>(0x100 | color.g),
                      static_cast<uint16_t>(0x100 | color.b) };
    spi_tx9(px, 3);
    cs_high();
}

void draw_char(uint16_t x, uint16_t y, char ch, Color fg, Color bg) {
    if (x + font::CHAR_W > WIDTH || y + font::CHAR_H > HEIGHT) return;
    if (ch < font::FIRST_CHAR || ch > font::LAST_CHAR) ch = ' ';

    const uint8_t* glyph = font::glyphs[ch - font::FIRST_CHAR];

    // Build all 8x16 pixels into line_buf (8*16*3 = 384 frames, fits in 960)
    uint16_t idx = 0;
    for (uint8_t row = 0; row < font::CHAR_H; row++) {
        uint8_t bits = glyph[row];
        for (uint8_t col = 0; col < font::CHAR_W; col++) {
            const Color& c = (bits & 0x80) ? fg : bg;
            line_buf[idx++] = 0x100 | c.r;
            line_buf[idx++] = 0x100 | c.g;
            line_buf[idx++] = 0x100 | c.b;
            bits <<= 1;
        }
    }

    begin_pixel_write(x, y, x + font::CHAR_W - 1, y + font::CHAR_H - 1);
    spi_tx9(line_buf, idx);
    cs_high();
}

void draw_string(uint16_t x, uint16_t y, const char* str, Color fg, Color bg) {
    while (*str) {
        if (x + font::CHAR_W > WIDTH) {
            x = 0;
            y += font::CHAR_H;
            if (y + font::CHAR_H > HEIGHT) return;
        }
        draw_char(x, y, *str++, fg, bg);
        x += font::CHAR_W;
    }
}

void draw_sprite(uint16_t x, uint16_t y, const Sprite& sprite, Color bg) {
    uint16_t w = sprite.width;
    uint16_t h = sprite.height;
    if (x + w > WIDTH || y + h > HEIGHT || w == 0 || h == 0) return;

    begin_pixel_write(x, y, x + w - 1, y + h - 1);

    const uint8_t* px = sprite.pixels;
    const uint8_t* alpha = sprite.alpha;

    for (uint16_t row = 0; row < h; row++) {
        uint16_t idx = 0;
        for (uint16_t col = 0; col < w; col++) {
            bool opaque = true;
            if (alpha) {
                uint16_t bit_idx = row * w + col;
                opaque = (alpha[bit_idx >> 3] >> (7 - (bit_idx & 7))) & 1;
            }
            if (opaque) {
                line_buf[idx++] = 0x100 | px[0];
                line_buf[idx++] = 0x100 | px[1];
                line_buf[idx++] = 0x100 | px[2];
            } else {
                line_buf[idx++] = 0x100 | bg.r;
                line_buf[idx++] = 0x100 | bg.g;
                line_buf[idx++] = 0x100 | bg.b;
            }
            px += 3;
        }
        spi_tx9(line_buf, idx);
    }

    cs_high();
}

} // namespace lcd
