#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_conf.h"

#define USBD_LANGID_STRING         0x0409U
#define USBD_MANUFACTURER_STRING   "STMicroelectronics"
#define USBD_PRODUCT_STRING        "STM32 Virtual COM Port"
#define USBD_SERIALNUMBER_STRING   "00000000001C"
#define USBD_CONFIGURATION_STRING  "CDC Config"
#define USBD_INTERFACE_STRING      "CDC Interface"

static uint8_t *CDC_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
static uint8_t *CDC_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
static uint8_t *CDC_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
static uint8_t *CDC_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
static uint8_t *CDC_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
static uint8_t *CDC_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
static uint8_t *CDC_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);

USBD_DescriptorsTypeDef CDC_Desc =
{
    CDC_DeviceDescriptor,
    CDC_LangIDStrDescriptor,
    CDC_ManufacturerStrDescriptor,
    CDC_ProductStrDescriptor,
    CDC_SerialStrDescriptor,
    CDC_ConfigStrDescriptor,
    CDC_InterfaceStrDescriptor,
};

__ALIGN_BEGIN static uint8_t hUSBDDeviceDesc[USB_LEN_DEV_DESC] __ALIGN_END =
{
    0x12,
    USB_DESC_TYPE_DEVICE,
    0x00, 0x02,               /* bcdUSB 2.0 */
    0x02,                     /* bDeviceClass: CDC */
    0x00,
    0x00,
    USB_MAX_EP0_SIZE,
    LOBYTE(USBD_VID),
    HIBYTE(USBD_VID),
    LOBYTE(USBD_PID_CDC),
    HIBYTE(USBD_PID_CDC),
    0x00, 0x02,               /* bcdDevice 2.00 */
    USBD_IDX_MFC_STR,
    USBD_IDX_PRODUCT_STR,
    USBD_IDX_SERIAL_STR,
    USBD_MAX_NUM_CONFIGURATION,
};

__ALIGN_BEGIN static uint8_t USBD_LangIDDesc[USB_LEN_LANGID_STR_DESC] __ALIGN_END =
{
    USB_LEN_LANGID_STR_DESC,
    USB_DESC_TYPE_STRING,
    LOBYTE(USBD_LANGID_STRING),
    HIBYTE(USBD_LANGID_STRING),
};

__ALIGN_BEGIN static uint8_t USBD_StrDesc[USBD_MAX_STR_DESC_SIZ] __ALIGN_END;

static uint8_t *CDC_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
    (void)speed;
    *length = sizeof(hUSBDDeviceDesc);
    return hUSBDDeviceDesc;
}

static uint8_t *CDC_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
    (void)speed;
    *length = sizeof(USBD_LangIDDesc);
    return USBD_LangIDDesc;
}

static uint8_t *CDC_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
    (void)speed;
    USBD_GetString((uint8_t *)USBD_MANUFACTURER_STRING, USBD_StrDesc, length);
    return USBD_StrDesc;
}

static uint8_t *CDC_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
    (void)speed;
    USBD_GetString((uint8_t *)USBD_PRODUCT_STRING, USBD_StrDesc, length);
    return USBD_StrDesc;
}

static uint8_t *CDC_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
    (void)speed;
    USBD_GetString((uint8_t *)USBD_SERIALNUMBER_STRING, USBD_StrDesc, length);
    return USBD_StrDesc;
}

static uint8_t *CDC_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
    (void)speed;
    USBD_GetString((uint8_t *)USBD_CONFIGURATION_STRING, USBD_StrDesc, length);
    return USBD_StrDesc;
}

static uint8_t *CDC_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
    (void)speed;
    USBD_GetString((uint8_t *)USBD_INTERFACE_STRING, USBD_StrDesc, length);
    return USBD_StrDesc;
}
