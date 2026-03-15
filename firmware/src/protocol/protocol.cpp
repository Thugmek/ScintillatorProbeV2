#include "protocol/protocol.h"
#include "protocol/messages.h"
#include "protocol/cobs.h"
#include "usb/usbd_cdc_if.h"
#include "logging/log.hpp"
#include <cstring>

LOG_COMPONENT_DEF(Proto, logging::Severity::debug);

namespace protocol {

static constexpr size_t RX_BUF_SIZE = 128;
static uint8_t rx_buf[RX_BUF_SIZE];
static size_t rx_pos = 0;

static uint16_t target_power_voltage = 0;

static void dispatch(const uint8_t *payload, size_t len) {
    if (len < 1) return;

    MsgId id = static_cast<MsgId>(payload[0]);
    switch (id) {
        case MsgId::TARGET_POWER: {
            if (len < sizeof(TargetPowerMsg)){
                log_info(Proto, "TargetPower: invalid message len.");
                return;
            }
            TargetPowerMsg msg;
            memcpy(&msg, payload, sizeof(msg));
            target_power_voltage = msg.voltage;
            log_info(Proto, "TargetPower: %u V", msg.voltage);
            break;
        }
        default:
            log_debug(Proto, "Unknown msg id: 0x%02X", payload[0]);
            break;
    }
}

void on_rx_bytes(const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        if (data[i] == 0x00) {
            // Frame delimiter — decode accumulated bytes
            if (rx_pos > 0) {
                uint8_t decoded[RX_BUF_SIZE];
                size_t decoded_len = cobs::decode(rx_buf, rx_pos, decoded);
                if (decoded_len > 0) {
                    dispatch(decoded, decoded_len);
                }
                rx_pos = 0;
            }
        } else {
            if (rx_pos < RX_BUF_SIZE) {
                rx_buf[rx_pos++] = data[i];
            } else {
                // Overflow — discard frame
                rx_pos = 0;
            }
        }
    }
}

void send_voltage_sense(uint16_t voltage) {
    VoltageSenseMsg msg;
    msg.msg_id = MsgId::VOLTAGE_SENSE;
    msg.voltage = voltage;

    uint8_t encoded[sizeof(msg) + 2 + 1]; // COBS overhead + delimiter
    size_t enc_len = cobs::encode(reinterpret_cast<const uint8_t *>(&msg),
                                  sizeof(msg), encoded);
    encoded[enc_len++] = 0x00; // frame delimiter

    usb_cdc_transmit(encoded, static_cast<uint16_t>(enc_len));
}

} // namespace protocol
