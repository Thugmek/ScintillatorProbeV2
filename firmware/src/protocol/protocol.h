#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cstdint>
#include <cstddef>

namespace protocol {

void on_rx_bytes(const uint8_t *data, size_t len);
void send_voltage_sense(uint16_t voltage);

} // namespace protocol

#endif /* PROTOCOL_H */
