#ifndef MESSAGES_H
#define MESSAGES_H

#include <cstdint>

enum class MsgId : uint8_t {
    TARGET_POWER  = 0x01,  // PC -> MCU
    VOLTAGE_SENSE = 0x02,  // MCU -> PC
};

struct __attribute__((packed)) TargetPowerMsg {
    MsgId msg_id;
    uint16_t voltage;  // volts
};

struct __attribute__((packed)) VoltageSenseMsg {
    MsgId msg_id;
    uint16_t voltage;  // volts
};

#endif /* MESSAGES_H */
