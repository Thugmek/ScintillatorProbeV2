#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cstdint>
#include <cstddef>

namespace protocol {

void on_rx_bytes(const uint8_t *data, size_t len);
void send_voltage_sense(uint16_t voltage);
void send_capture_data(const uint32_t *samples, uint16_t count);

} // namespace protocol

#endif /* PROTOCOL_H */
