#include "capture.hpp"
#include "hal.hpp"

namespace capture {

volatile State state = State::ARMED;
uint32_t dma_buffer[BUFFER_SIZE];
uint32_t result[CAPTURE_SIZE];

static volatile uint16_t trigger_pos;

void on_watchdog() {
    if (state != State::ARMED) return;

    // Record DMA write position at the moment of trigger.
    // NDTR counts remaining transfers; subtract from buffer size to get index.
    uint16_t ndtr = __HAL_DMA_GET_COUNTER(&hal::hdma_adc1);
    trigger_pos = (BUFFER_SIZE - ndtr) % BUFFER_SIZE;

    // Disable AWD interrupt to prevent re-triggering during capture
    __HAL_ADC_DISABLE_IT(&hal::hadc1, ADC_IT_AWD1);

    state = State::TRIGGERED;
}

void on_dma_event(uint16_t current_pos) {
    if (state != State::TRIGGERED) return;

    uint16_t elapsed = (current_pos - trigger_pos + BUFFER_SIZE) % BUFFER_SIZE;
    if (elapsed < POST_TRIGGER) return;

    // Copy the capture window (pre-trigger + post-trigger) from ring buffer
    uint16_t start = (trigger_pos - PRE_TRIGGER + BUFFER_SIZE) % BUFFER_SIZE;
    for (uint16_t i = 0; i < CAPTURE_SIZE; i++) {
        result[i] = dma_buffer[(start + i) % BUFFER_SIZE];
    }

    state = State::CAPTURED;
}

bool is_ready() {
    return state == State::CAPTURED;
}

void arm() {
    __HAL_ADC_ENABLE_IT(&hal::hadc1, ADC_IT_AWD1);
    state = State::ARMED;
}

} // namespace capture
