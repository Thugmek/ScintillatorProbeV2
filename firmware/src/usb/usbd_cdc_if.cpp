#include "usbd_cdc_if.h"
#include "usbd_cdc.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "protocol/protocol.h"

extern "C" {

static USBD_HandleTypeDef hUsbDeviceFS;

#define CDC_RX_DATA_SIZE  256U
#define CDC_TX_DATA_SIZE  256U

static uint8_t UserRxBufferFS[CDC_RX_DATA_SIZE];
static uint8_t UserTxBufferFS[CDC_TX_DATA_SIZE];

static volatile bool tx_busy = false;

static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t *pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t *Buf, uint32_t *Len);
static int8_t CDC_TransmitCplt_FS(uint8_t *Buf, uint32_t *Len, uint8_t epnum);

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS = {
    CDC_Init_FS,
    CDC_DeInit_FS,
    CDC_Control_FS,
    CDC_Receive_FS,
    CDC_TransmitCplt_FS
};

static int8_t CDC_Init_FS(void) {
    USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
    tx_busy = false;
    return USBD_OK;
}

static int8_t CDC_DeInit_FS(void) {
    return USBD_OK;
}

static int8_t CDC_Control_FS(uint8_t cmd, uint8_t *pbuf, uint16_t length) {
    (void)pbuf;
    (void)length;
    switch (cmd) {
        case CDC_SET_LINE_CODING:
        case CDC_GET_LINE_CODING:
        case CDC_SET_CONTROL_LINE_STATE:
        case CDC_SEND_BREAK:
        default:
            break;
    }
    return USBD_OK;
}

static int8_t CDC_Receive_FS(uint8_t *Buf, uint32_t *Len) {
    protocol::on_rx_bytes(Buf, *Len);
    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
    USBD_CDC_ReceivePacket(&hUsbDeviceFS);
    return USBD_OK;
}

static int8_t CDC_TransmitCplt_FS(uint8_t *Buf, uint32_t *Len, uint8_t epnum) {
    (void)Buf;
    (void)Len;
    (void)epnum;
    tx_busy = false;
    return USBD_OK;
}

} // extern "C"

void usb_cdc_init(void) {
    USBD_Init(&hUsbDeviceFS, &FS_Desc, 0);
    USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC);
    USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS);
    USBD_Start(&hUsbDeviceFS);
}

bool usb_cdc_transmit(const uint8_t *buf, uint16_t len) {
    if (tx_busy) return false;
    if (hUsbDeviceFS.dev_state != USBD_STATE_CONFIGURED) return false;

    USBD_CDC_SetTxBuffer(&hUsbDeviceFS, const_cast<uint8_t *>(buf), len);
    tx_busy = true;
    if (USBD_CDC_TransmitPacket(&hUsbDeviceFS) != USBD_OK) {
        tx_busy = false;
        return false;
    }
    return true;
}

bool usb_cdc_is_connected(void) {
    return hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED;
}
