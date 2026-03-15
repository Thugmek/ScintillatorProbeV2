#pragma once
#include <cstdint>
#include "FreeRTOS.h"
#include "task.h"

namespace capture {

// Task handle for the capture processing task (set by main at task creation).
// ISRs use this to send notifications when a capture completes.
extern TaskHandle_t task_handle;

// Circular DMA buffer size in uint32_t words.
// Must be large enough that pre-trigger data is not overwritten before we copy it.
// With half-transfer callbacks every BUFFER_SIZE/2 words, worst-case delay before
// copy is ~BUFFER_SIZE/2 words. Need: BUFFER_SIZE/2 + PRE_TRIGGER < BUFFER_SIZE.
inline constexpr uint16_t BUFFER_SIZE = 256;

// With ADC_DUALMODEDATAFORMAT_32_10_BITS, each DMA word packs 2 interleaved samples:
//   [15:0] = ADC1 (master), [31:16] = ADC2 (slave)

// DMA words before trigger (8 words = 16 samples)
inline constexpr uint16_t PRE_TRIGGER = 8;

// DMA words after trigger (32 words = 64 samples)
inline constexpr uint16_t POST_TRIGGER = 32;

inline constexpr uint16_t CAPTURE_SIZE = PRE_TRIGGER + POST_TRIGGER;

// Total individual samples in a capture (for unpacking/display)
inline constexpr uint16_t CAPTURE_SAMPLES = CAPTURE_SIZE * 2;

// Analog watchdog threshold (raw ADC counts, 14-bit range: 0–16383)
inline constexpr uint16_t TRIGGER_THRESHOLD = 512;

enum class State : uint8_t {
    ARMED,
    TRIGGERED,
    CAPTURED,
};

// DMA writes into this circular buffer
extern uint32_t dma_buffer[BUFFER_SIZE];

// Snapshot of the capture window, copied from dma_buffer on trigger
extern uint32_t result[CAPTURE_SIZE];

extern volatile State state;

// Called from ADC AWD ISR — records trigger position, disables AWD
void on_watchdog();

// Called from DMA half/complete ISR — checks if enough post-trigger data collected
void on_dma_event(uint16_t current_pos);

// True when a capture is ready for processing
bool is_ready();

// Re-arm the trigger after processing (re-enables AWD interrupt)
void arm();

} // namespace capture
