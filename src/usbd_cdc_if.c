#include "usbd_cdc_if.h"
#include "usbd_core.h"

extern USBD_HandleTypeDef husbd;

#define CDC_RX_BUF_SIZE 256U

static uint8_t UserRxBuffer[CDC_RX_BUF_SIZE];

static int8_t CDC_Init(void);
static int8_t CDC_DeInit(void);
static int8_t CDC_Control(uint8_t cmd, uint8_t *pbuf, uint16_t length);
static int8_t CDC_Receive(uint8_t *pbuf, uint32_t *len);
static int8_t CDC_TransmitCplt(uint8_t *pbuf, uint32_t *len, uint8_t epnum);

USBD_CDC_ItfTypeDef USBD_CDC_fops =
{
    CDC_Init,
    CDC_DeInit,
    CDC_Control,
    CDC_Receive,
    CDC_TransmitCplt,
};

static int8_t CDC_Init(void)
{
    USBD_CDC_SetRxBuffer(&husbd, UserRxBuffer);
    return USBD_OK;
}

static int8_t CDC_DeInit(void)
{
    return USBD_OK;
}

static int8_t CDC_Control(uint8_t cmd, uint8_t *pbuf, uint16_t length)
{
    (void)cmd;
    (void)pbuf;
    (void)length;
    return USBD_OK;
}

static int8_t CDC_Receive(uint8_t *pbuf, uint32_t *len)
{
    (void)pbuf;
    (void)len;
    USBD_CDC_SetRxBuffer(&husbd, UserRxBuffer);
    USBD_CDC_ReceivePacket(&husbd);
    return USBD_OK;
}

static int8_t CDC_TransmitCplt(uint8_t *pbuf, uint32_t *len, uint8_t epnum)
{
    (void)pbuf;
    (void)len;
    (void)epnum;
    return USBD_OK;
}

USBD_StatusTypeDef USB_CDC_Transmit(uint8_t *buf, uint16_t len)
{
    USBD_CDC_HandleTypeDef *hcdc;
    uint32_t timeout;

    if (husbd.dev_state != USBD_STATE_CONFIGURED)
    {
        return USBD_BUSY;
    }

    hcdc = (USBD_CDC_HandleTypeDef *)husbd.pClassData;
    if (hcdc == NULL)
    {
        return USBD_FAIL;
    }

    timeout = 100000U;
    while (hcdc->TxState != 0U)
    {
        if (timeout == 0U)
        {
            return USBD_BUSY;
        }
        timeout--;
    }

    USBD_CDC_SetTxBuffer(&husbd, buf, len);
    return USBD_CDC_TransmitPacket(&husbd);
}
