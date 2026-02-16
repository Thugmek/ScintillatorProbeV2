#include "lcd/ili9488.hpp"
#include "hal.hpp"
#include "logging/log.hpp"

LOG_COMPONENT_DEF(LCD, logging::Severity::debug);

namespace lcd {

// ILI9488 command definitions
enum Cmd : uint8_t {
    NOP     = 0x00,
    SWRESET = 0x01,
    RDDID   = 0xD3,
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

// Line buffer for efficient fills: one full row = 320 pixels * 3 bytes
static uint8_t line_buf[WIDTH * 3];

// --- Pin helpers ---
static bool cs_inverted = false;
static bool dc_inverted = false;

static inline void cs_assert()   { HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, cs_inverted ? GPIO_PIN_SET : GPIO_PIN_RESET); }
static inline void cs_deassert() { HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, cs_inverted ? GPIO_PIN_RESET : GPIO_PIN_SET); }
static inline void dc_low()  { HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, dc_inverted ? GPIO_PIN_SET : GPIO_PIN_RESET); }
static inline void dc_high() { HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, dc_inverted ? GPIO_PIN_RESET : GPIO_PIN_SET); }
static inline void rst_low() { HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET); }
static inline void rst_high(){ HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET); }

// --- Bitbang SPI ---

static inline void bb_sck_low()  { GPIOB->BSRR = GPIO_PIN_3 << 16; }
static inline void bb_sck_high() { GPIOB->BSRR = GPIO_PIN_3; }
static inline void bb_mosi_low() { GPIOB->BSRR = GPIO_PIN_5 << 16; }
static inline void bb_mosi_high(){ GPIOB->BSRR = GPIO_PIN_5; }

static void bb_delay() {
    for (volatile int i = 0; i < 20; i++);
}

static void bb_init_gpio() {
    __HAL_SPI_DISABLE(&hal::hspi1);
    SPI1->CR1 &= ~SPI_CR1_SPE;

    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = GPIO_PIN_3 | GPIO_PIN_5;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &gpio);

    bb_sck_low();
    bb_mosi_low();
}

// Bitbang a single bit on MOSI + clock it
static inline void bb_bit(bool val) {
    if (val) bb_mosi_high(); else bb_mosi_low();
    bb_delay();
    bb_sck_high();
    bb_delay();
    bb_sck_low();
}

// === 4-wire SPI: 8 bits per frame, DC on separate pin ===

static void bb4_send_byte(uint8_t data) {
    for (int bit = 7; bit >= 0; bit--)
        bb_bit(data & (1 << bit));
}

static void bb4_cmd(uint8_t cmd) {
    cs_assert(); dc_low();
    bb4_send_byte(cmd);
    cs_deassert();
}

static void bb4_cmd_data(uint8_t cmd, const uint8_t* data, uint16_t len) {
    cs_assert(); dc_low();
    bb4_send_byte(cmd);
    dc_high();
    for (uint16_t i = 0; i < len; i++) bb4_send_byte(data[i]);
    cs_deassert();
}

static void bb4_cmd8(uint8_t cmd, uint8_t val) {
    bb4_cmd_data(cmd, &val, 1);
}

// === 3-wire SPI: 9 bits per frame, DC is MSB (bit 8) ===
// Bit 8 = 0: command, Bit 8 = 1: data

static void bb3_send9(bool dc, uint8_t data) {
    bb_bit(dc);  // DC as 9th bit (MSB)
    for (int bit = 7; bit >= 0; bit--)
        bb_bit(data & (1 << bit));
}

static void bb3_cmd(uint8_t cmd) {
    cs_assert();
    bb3_send9(false, cmd);
    cs_deassert();
}

static void bb3_cmd_data(uint8_t cmd, const uint8_t* data, uint16_t len) {
    cs_assert();
    bb3_send9(false, cmd);
    for (uint16_t i = 0; i < len; i++) bb3_send9(true, data[i]);
    cs_deassert();
}

static void bb3_cmd8(uint8_t cmd, uint8_t val) {
    bb3_cmd_data(cmd, &val, 1);
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

// --- Bare minimum init + red block, parameterized by wire mode ---

static void test_init_and_draw(bool three_wire) {
    auto cmd  = three_wire ? bb3_cmd  : bb4_cmd;
    auto cmd8 = three_wire ? bb3_cmd8 : bb4_cmd8;

    hw_reset();
    cmd(SWRESET);      HAL_Delay(120);
    cmd(SLPOUT);       HAL_Delay(120);
    cmd8(PIXFMT, 0x66);
    cmd(DISPON);       HAL_Delay(20);

    // CASET 0..49
    { uint8_t d[] = {0,0,0,49}; three_wire ? bb3_cmd_data(CASET,d,4) : bb4_cmd_data(CASET,d,4); }
    // RASET 0..49
    { uint8_t d[] = {0,0,0,49}; three_wire ? bb3_cmd_data(RASET,d,4) : bb4_cmd_data(RASET,d,4); }

    // RAMWR + 50x50 red pixels
    cs_assert();
    if (three_wire) {
        bb3_send9(false, RAMWR);
        for (int i = 0; i < 50*50; i++) {
            bb3_send9(true, 0xFF);
            bb3_send9(true, 0x00);
            bb3_send9(true, 0x00);
        }
    } else {
        dc_low();  bb4_send_byte(RAMWR);
        dc_high();
        for (int i = 0; i < 50*50; i++) {
            bb4_send_byte(0xFF);
            bb4_send_byte(0x00);
            bb4_send_byte(0x00);
        }
    }
    cs_deassert();
}

// --- Public API ---

bool test_connection() {
    log_info(LCD, "=== POLARITY TEST ===");
    bb_init_gpio();

    // ---- Test 1: 4-wire, normal CS, normal DC ----
    cs_inverted = false; dc_inverted = false;
    cs_deassert();
    log_info(LCD, "TEST 1: 4-wire, CS normal, DC normal");
    test_init_and_draw(false);
    log_info(LCD, "  -> square? Waiting 3s...");
    HAL_Delay(3000);

    // ---- Test 2: 4-wire, normal CS, inverted DC ----
    cs_inverted = false; dc_inverted = true;
    cs_deassert();
    log_info(LCD, "TEST 2: 4-wire, CS normal, DC inverted");
    test_init_and_draw(false);
    log_info(LCD, "  -> square? Waiting 3s...");
    HAL_Delay(3000);

    // ---- Test 3: 4-wire, inverted CS, normal DC ----
    cs_inverted = true; dc_inverted = false;
    cs_deassert();
    log_info(LCD, "TEST 3: 4-wire, CS inverted, DC normal");
    test_init_and_draw(false);
    log_info(LCD, "  -> square? Waiting 3s...");
    HAL_Delay(3000);

    // ---- Test 4: 4-wire, inverted CS, inverted DC ----
    cs_inverted = true; dc_inverted = true;
    cs_deassert();
    log_info(LCD, "TEST 4: 4-wire, CS inverted, DC inverted");
    test_init_and_draw(false);
    log_info(LCD, "  -> square? Waiting 3s...");
    HAL_Delay(3000);

    // ---- Test 5: 3-wire, normal CS (previously worked) ----
    cs_inverted = false; dc_inverted = false;
    cs_deassert();
    log_info(LCD, "TEST 5: 3-wire, CS normal (was working)");
    test_init_and_draw(true);
    log_info(LCD, "  -> square? Waiting 3s...");
    HAL_Delay(3000);

    // Restore
    cs_inverted = false; dc_inverted = false;
    cs_deassert();
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = GPIO_PIN_3 | GPIO_PIN_5;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOB, &gpio);
    HAL_SPI_Init(&hal::hspi1);

    log_info(LCD, "=== done ===");
    return true;
}

// --- HW SPI helpers (for normal operation after test) ---

static HAL_StatusTypeDef spi_tx(const uint8_t* data, uint16_t len) {
    return HAL_SPI_Transmit(&hal::hspi1, data, len, HAL_MAX_DELAY);
}

static void write_cmd_data(uint8_t cmd, const uint8_t* data, uint16_t len) {
    cs_assert(); dc_low();
    spi_tx(&cmd, 1);
    dc_high();
    spi_tx(data, len);
    cs_deassert();
}

static void write_cmd(uint8_t cmd) {
    cs_assert(); dc_low();
    spi_tx(&cmd, 1);
    cs_deassert();
}

static void write_cmd8(uint8_t cmd, uint8_t val) {
    write_cmd_data(cmd, &val, 1);
}

static void begin_pixel_write(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    { uint8_t d[] = { static_cast<uint8_t>(x0>>8), static_cast<uint8_t>(x0),
                      static_cast<uint8_t>(x1>>8), static_cast<uint8_t>(x1) };
      write_cmd_data(CASET, d, 4); }
    { uint8_t d[] = { static_cast<uint8_t>(y0>>8), static_cast<uint8_t>(y0),
                      static_cast<uint8_t>(y1>>8), static_cast<uint8_t>(y1) };
      write_cmd_data(RASET, d, 4); }
    cs_assert(); dc_low();
    uint8_t cmd = RAMWR;
    spi_tx(&cmd, 1);
    dc_high();
}

void init() {
    hw_reset();
    write_cmd(SWRESET); HAL_Delay(120);
    { const uint8_t d[] = {0x00,0x13,0x18,0x04,0x0F,0x06,0x3A,0x56,
                           0x4D,0x03,0x0A,0x06,0x30,0x3E,0x0F};
      write_cmd_data(PGAMMA, d, sizeof(d)); }
    { const uint8_t d[] = {0x00,0x13,0x18,0x01,0x11,0x06,0x38,0x34,
                           0x4D,0x06,0x0D,0x0B,0x31,0x37,0x0F};
      write_cmd_data(NGAMMA, d, sizeof(d)); }
    { const uint8_t d[] = {0x18, 0x16}; write_cmd_data(PWCTR1, d, 2); }
    write_cmd8(PWCTR2, 0x45);
    { const uint8_t d[] = {0x00, 0x63, 0x01}; write_cmd_data(VMCTR, d, 3); }
    write_cmd8(MADCTL, 0x48);
    write_cmd8(PIXFMT, 0x66);
    write_cmd8(IFMODE, 0x80);
    { const uint8_t d[] = {0x00, 0x10}; write_cmd_data(FRMCTR, d, 2); }
    write_cmd8(INVCTR, 0x02);
    { const uint8_t d[] = {0x02, 0x02}; write_cmd_data(DISCTRL, d, 2); }
    write_cmd8(IMGFUNC, 0x00);
    { const uint8_t d[] = {0xA9, 0x51, 0x2C, 0x82}; write_cmd_data(ADJCTRL, d, 4); }
    write_cmd(SLPOUT); HAL_Delay(120);
    write_cmd(INVON);
    write_cmd(DISPON); HAL_Delay(20);
    fill_screen(colors::GREEN);
}

void fill_screen(Color color) { fill_rect(0, 0, WIDTH, HEIGHT, color); }

void fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, Color color) {
    if (x >= WIDTH || y >= HEIGHT || w == 0 || h == 0) return;
    if (x + w > WIDTH)  w = WIDTH - x;
    if (y + h > HEIGHT) h = HEIGHT - y;
    uint16_t row_bytes = w * 3;
    for (uint16_t i = 0; i < w; i++) {
        line_buf[i*3+0] = color.r; line_buf[i*3+1] = color.g; line_buf[i*3+2] = color.b;
    }
    begin_pixel_write(x, y, x + w - 1, y + h - 1);
    for (uint16_t row = 0; row < h; row++) spi_tx(line_buf, row_bytes);
    cs_deassert();
}

void draw_pixel(uint16_t x, uint16_t y, Color color) {
    if (x >= WIDTH || y >= HEIGHT) return;
    begin_pixel_write(x, y, x, y);
    uint8_t d[] = {color.r, color.g, color.b};
    spi_tx(d, 3);
    cs_deassert();
}

} // namespace lcd
