#ifndef USBD_CDC_IF_H
#define USBD_CDC_IF_H

#include <cstdint>
#include <cstddef>

void usb_cdc_init(void);
bool usb_cdc_transmit(const uint8_t *buf, uint16_t len);
bool usb_cdc_is_connected(void);

#endif /* USBD_CDC_IF_H */
