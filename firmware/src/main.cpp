#include <cstdint>
#include "hal.hpp"
#include "capture.hpp"
#include "logging/log.hpp"
#include "usb/usbd_cdc_if.h"
#include "protocol/protocol.h"

LOG_COMPONENT_DEF(Main, logging::Severity::debug);

int main() {
    hal::init();
    logging::init();
    usb_cdc_init();

    log_info(Main, "Firmware started");

    HAL_TIM_PWM_Start(&hal::htim2, TIM_CHANNEL_3);
    log_info(Main, "PWM started on TIM2_CH3");

    HAL_ADC_Start(&hal::hadc2);
    HAL_ADCEx_MultiModeStart_DMA(&hal::hadc1, capture::dma_buffer, capture::BUFFER_SIZE);
    capture::arm();
    log_info(Main, "Capture armed (threshold=%u)", capture::TRIGGER_THRESHOLD);

    uint32_t last_voltage_tick = 0;

    while (true) {
        // Send voltage sense periodically (every 1 second)
        uint32_t now = HAL_GetTick();
        if (now - last_voltage_tick >= 1000) {
            last_voltage_tick = now;
            if (usb_cdc_is_connected()) {
                protocol::send_voltage_sense(0); // placeholder: actual voltage TBD
            }
        }

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

extern "C" void OTG_FS_IRQHandler(void) {
    HAL_PCD_IRQHandler(&hal::hpcd_USB_OTG_FS);
}

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
