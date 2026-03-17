#include <cstdint>
#include "FreeRTOS.h"
#include "task.h"
#include "hal.hpp"
#include "capture.hpp"
#include "logging/log.hpp"
#include "usb/usbd_cdc_if.h"
#include "protocol/protocol.h"

LOG_COMPONENT_DEF(Main, logging::Severity::debug);

// ---------------------------------------------------------------------------
// Task: peak capture
// Blocks on a notification from the DMA ISR, processes the captured waveform,
// then re-arms the trigger.
// ---------------------------------------------------------------------------
static void capture_task(void *) {
    HAL_ADC_Start(&hal::hadc2);
    HAL_ADCEx_MultiModeStart_DMA(&hal::hadc1, capture::dma_buffer, capture::BUFFER_SIZE);
    capture::arm();
    log_info(Main, "Capture armed (threshold=%u)", capture::TRIGGER_THRESHOLD);

    for (;;) {
        // Block until the DMA ISR signals a completed capture
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (!capture::is_ready()) continue;

        if (usb_cdc_is_connected()) {
            protocol::send_capture_data(capture::result, capture::CAPTURE_SIZE);
        }

        capture::arm();
    }
}

// ---------------------------------------------------------------------------
// Task: USB COBS communication
// Periodically sends voltage-sense telemetry over the USB CDC link.
// ---------------------------------------------------------------------------
static void usb_cobs_task(void *) {
    for (;;) {
        // Poll ADC3 channel 9 (voltage sense)
        HAL_ADC_Start(&hal::hadc3);
        HAL_ADC_PollForConversion(&hal::hadc3, 10);
        uint32_t adc_raw = HAL_ADC_GetValue(&hal::hadc3);
        HAL_ADC_Stop(&hal::hadc3);

        // 16-bit ADC, Vref=3.3V, divider ratio 1001:1
        // voltage_V = adc_raw / 65535 * 3.3 * 1001
        uint16_t voltage = static_cast<uint16_t>(adc_raw * 3303U / 65535U);

        if (usb_cdc_is_connected()) {
            protocol::send_voltage_sense(voltage);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

// ---------------------------------------------------------------------------
// SysTick & FreeRTOS hooks
// ---------------------------------------------------------------------------
// HAL_Init() starts SysTick before the scheduler is running, so we must guard
// the FreeRTOS tick handler until vTaskStartScheduler() has been called.
extern "C" void xPortSysTickHandler(void);

extern "C" void SysTick_Handler(void) {
    HAL_IncTick();
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        xPortSysTickHandler();
    }
}

extern "C" void vApplicationStackOverflowHook(TaskHandle_t, char *) {
    hal::panic();
}

extern "C" void vApplicationMallocFailedHook(void) {
    hal::panic();
}

// ---------------------------------------------------------------------------
// main — create tasks and start the scheduler
// ---------------------------------------------------------------------------
int main() {
    hal::init();
    logging::init();
    usb_cdc_init();

    log_info(Main, "Firmware started");

    HAL_TIM_PWM_Start(&hal::htim2, TIM_CHANNEL_3);
    log_info(Main, "PWM started on TIM2_CH3");

    xTaskCreate(capture_task,  "capture",  512, nullptr, 3, &capture::task_handle);
    xTaskCreate(usb_cobs_task, "usb_cobs", 512, nullptr, 2, nullptr);

    vTaskStartScheduler();

    // Should never reach here
    for (;;) {}
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
