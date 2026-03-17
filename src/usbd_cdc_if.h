#ifndef USBD_CDC_IF_H
#define USBD_CDC_IF_H

#include "usbd_cdc.h"

extern USBD_CDC_ItfTypeDef USBD_CDC_fops;

USBD_StatusTypeDef USB_CDC_Transmit(uint8_t *buf, uint16_t len);

#endif
