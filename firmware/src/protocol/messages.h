#ifndef MESSAGES_H
#define MESSAGES_H

#include <cstdint>
#include "capture.hpp"

enum class MsgId : uint8_t {
    TARGET_POWER   = 0x01,  // PC -> MCU
    VOLTAGE_SENSE  = 0x02,  // MCU -> PC
    CAPTURE_DATA   = 0x03,  // MCU -> PC
};

struct __attribute__((packed)) TargetPowerMsg {
    MsgId msg_id;
    uint16_t voltage;  // volts
};

struct __attribute__((packed)) VoltageSenseMsg {
    MsgId msg_id;
    uint16_t voltage;  // volts
};

struct __attribute__((packed)) CaptureDataMsg {
    MsgId msg_id;
    uint32_t samples[capture::CAPTURE_SIZE];  // interleaved ADC1/ADC2 words
};

#endif /* MESSAGES_H */
