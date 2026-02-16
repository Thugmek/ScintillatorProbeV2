#include <cstdint>
#include "hal.hpp"
#include "capture.hpp"
#include "logging/log.hpp"
#include "lcd/ili9488.hpp"

LOG_COMPONENT_DEF(Main, logging::Severity::debug);

static void lcd_demo() {
    lcd::test_connection();
    lcd::init();

    // Flash primary colors
    const lcd::Color flash[] = {lcd::colors::RED, lcd::colors::GREEN, lcd::colors::BLUE};
    for (auto c : flash) {
        lcd::fill_screen(c);
        HAL_Delay(400);
    }

    // Black background
    lcd::fill_screen(lcd::colors::BLACK);

    // Draw 8 vertical color bars
    const lcd::Color bars[] = {
        lcd::colors::WHITE, lcd::colors::RED,     lcd::colors::GREEN,  lcd::colors::BLUE,
        lcd::colors::YELLOW, lcd::colors::CYAN,   lcd::colors::MAGENTA, lcd::colors::ORANGE
    };
    constexpr uint16_t bar_w = lcd::WIDTH / 8;
    for (int i = 0; i < 8; i++) {
        lcd::fill_rect(i * bar_w, 0, bar_w, lcd::HEIGHT / 2, bars[i]);
    }

    // Draw horizontal gradient below the bars
    for (uint16_t x = 0; x < lcd::WIDTH; x++) {
        uint8_t r = static_cast<uint8_t>(x * 255 / lcd::WIDTH);
        uint8_t b = static_cast<uint8_t>(255 - r);
        lcd::fill_rect(x, lcd::HEIGHT / 2, 1, lcd::HEIGHT / 2, {r, 0, b});
    }

    log_info(Main, "LCD: demo pattern drawn");
}

int main() {
    hal::init();
    logging::init();

    log_info(Main, "Firmware started");

    lcd::test_connection();
    // lcd_demo();

    HAL_TIM_PWM_Start(&hal::htim1, TIM_CHANNEL_1);
    log_info(Main, "PWM started on TIM1_CH1");

    HAL_ADC_Start(&hal::hadc2);
    HAL_ADCEx_MultiModeStart_DMA(&hal::hadc1, capture::dma_buffer, capture::BUFFER_SIZE);
    capture::arm();
    log_info(Main, "Capture armed (threshold=%u)", capture::TRIGGER_THRESHOLD);

    while (true) {
        if (capture::is_ready()) {
            // Unpack interleaved samples: each uint32_t has ADC1[15:0], ADC2[31:16]
            static char viz[capture::CAPTURE_SAMPLES + 1];
            for (uint16_t i = 0; i < capture::CAPTURE_SIZE; i++) {
                uint16_t adc1 = capture::result[i] & 0xFFFF;
                uint16_t adc2 = (capture::result[i] >> 16) & 0xFFFF;
                viz[i * 2]     = (adc1 > capture::TRIGGER_THRESHOLD) ? '*' : '.';
                viz[i * 2 + 1] = (adc2 > capture::TRIGGER_THRESHOLD) ? '*' : '.';
            }
            viz[capture::CAPTURE_SAMPLES] = 0;
            log_debug(Main, "%s", viz);

            capture::arm();
        }
    }
}

// --- Interrupt handlers & HAL callbacks ---

extern "C" void DMA1_Stream0_IRQHandler(void) {
    HAL_DMA_IRQHandler(hal::hadc1.DMA_Handle);
}

extern "C" void ADC_IRQHandler(void) {
    HAL_ADC_IRQHandler(&hal::hadc1);
}

extern "C" void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
        capture::on_watchdog();
    }
}

extern "C" void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
        // DMA just completed full buffer, write pointer wraps to 0
        capture::on_dma_event(0);
    }
}

extern "C" void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
        // DMA just completed first half, write pointer is at midpoint
        capture::on_dma_event(capture::BUFFER_SIZE / 2);
    }
}
