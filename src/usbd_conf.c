#include "stm32f4xx_hal.h"
#include "usbd_core.h"
#include "usbd_conf.h"

static PCD_HandleTypeDef hpcd;

void SysTick_Handler(void)
{
    HAL_IncTick();
}

void OTG_FS_IRQHandler(void)
{
    HAL_PCD_IRQHandler(&hpcd);
}

void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd_handle)
{
    GPIO_InitTypeDef gpio = {0};

    if (hpcd_handle->Instance == USB_OTG_FS)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /* PA11 = DM, PA12 = DP */
        gpio.Pin       = GPIO_PIN_11 | GPIO_PIN_12;
        gpio.Mode      = GPIO_MODE_AF_PP;
        gpio.Pull      = GPIO_NOPULL;
        gpio.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        gpio.Alternate = GPIO_AF10_OTG_FS;
        HAL_GPIO_Init(GPIOA, &gpio);

        /* PA9 = VBUS sensing (input) */
        gpio.Pin  = GPIO_PIN_9;
        gpio.Mode = GPIO_MODE_INPUT;
        gpio.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &gpio);

        /* PA10 = ID (OD, pull-up) */
        gpio.Pin       = GPIO_PIN_10;
        gpio.Mode      = GPIO_MODE_AF_OD;
        gpio.Pull      = GPIO_PULLUP;
        gpio.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        gpio.Alternate = GPIO_AF10_OTG_FS;
        HAL_GPIO_Init(GPIOA, &gpio);

        __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

        HAL_NVIC_SetPriority(OTG_FS_IRQn, 5U, 0U);
        HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
    }
}

void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd_handle)
{
    if (hpcd_handle->Instance == USB_OTG_FS)
    {
        __HAL_RCC_USB_OTG_FS_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);
        HAL_NVIC_DisableIRQ(OTG_FS_IRQn);
    }
}

/* PCD callbacks -> USBD LL */

void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd_handle)
{
    USBD_LL_SetupStage(hpcd_handle->pData, (uint8_t *)hpcd_handle->Setup);
}

void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd_handle, uint8_t epnum)
{
    USBD_LL_DataOutStage(hpcd_handle->pData, epnum, hpcd_handle->OUT_ep[epnum].xfer_buff);
}

void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd_handle, uint8_t epnum)
{
    USBD_LL_DataInStage(hpcd_handle->pData, epnum, hpcd_handle->IN_ep[epnum].xfer_buff);
}

void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd_handle)
{
    USBD_LL_SOF(hpcd_handle->pData);
}

void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd_handle)
{
    USBD_SpeedTypeDef speed = USBD_SPEED_FULL;

    if (hpcd_handle->Init.speed == PCD_SPEED_HIGH)
    {
        speed = USBD_SPEED_HIGH;
    }

    USBD_LL_SetSpeed(hpcd_handle->pData, speed);
    USBD_LL_Reset(hpcd_handle->pData);
}

void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd_handle)
{
    USBD_LL_Suspend(hpcd_handle->pData);
}

void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd_handle)
{
    USBD_LL_Resume(hpcd_handle->pData);
}

void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd_handle, uint8_t epnum)
{
    USBD_LL_IsoOUTIncomplete(hpcd_handle->pData, epnum);
}

void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd_handle, uint8_t epnum)
{
    USBD_LL_IsoINIncomplete(hpcd_handle->pData, epnum);
}

void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd_handle)
{
    USBD_LL_DevConnected(hpcd_handle->pData);
}

void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd_handle)
{
    USBD_LL_DevDisconnected(hpcd_handle->pData);
}

/* USBD LL interface */

USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef *pdev)
{
    NVIC_SetPriority(SysTick_IRQn, 0U);

    hpcd.Instance                 = USB_OTG_FS;
    hpcd.Init.dev_endpoints       = 4U;
    hpcd.Init.use_dedicated_ep1   = 0U;
    hpcd.Init.dma_enable          = 0U;
    hpcd.Init.low_power_enable    = 0U;
    hpcd.Init.phy_itface          = PCD_PHY_EMBEDDED;
    hpcd.Init.Sof_enable          = 0U;
    hpcd.Init.speed               = PCD_SPEED_FULL;
    hpcd.Init.vbus_sensing_enable = 1U;
    hpcd.pData                    = pdev;
    pdev->pData                   = &hpcd;

    (void)HAL_PCD_Init(&hpcd);

    HAL_PCDEx_SetRxFiFo(&hpcd, 0x80U);
    HAL_PCDEx_SetTxFiFo(&hpcd, 0U, 0x40U);
    HAL_PCDEx_SetTxFiFo(&hpcd, 1U, 0x80U);

    return USBD_OK;
}

USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef *pdev)
{
    (void)HAL_PCD_DeInit(pdev->pData);
    return USBD_OK;
}

USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef *pdev)
{
    (void)HAL_PCD_Start(pdev->pData);
    return USBD_OK;
}

USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef *pdev)
{
    (void)HAL_PCD_Stop(pdev->pData);
    return USBD_OK;
}

USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr,
                                   uint8_t ep_type, uint16_t ep_mps)
{
    (void)HAL_PCD_EP_Open(pdev->pData, ep_addr, ep_mps, ep_type);
    return USBD_OK;
}

USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    (void)HAL_PCD_EP_Close(pdev->pData, ep_addr);
    return USBD_OK;
}

USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    (void)HAL_PCD_EP_Flush(pdev->pData, ep_addr);
    return USBD_OK;
}

USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    (void)HAL_PCD_EP_SetStall(pdev->pData, ep_addr);
    return USBD_OK;
}

USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    (void)HAL_PCD_EP_ClrStall(pdev->pData, ep_addr);
    return USBD_OK;
}

uint8_t USBD_LL_IsStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    PCD_HandleTypeDef *phcd = (PCD_HandleTypeDef *)pdev->pData;

    if ((ep_addr >> 7U) & 0x01U)
    {
        return phcd->IN_ep[ep_addr & 0x7FU].is_stall;
    }
    return phcd->OUT_ep[ep_addr & 0x7FU].is_stall;
}

USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef *pdev, uint8_t dev_addr)
{
    (void)HAL_PCD_SetAddress(pdev->pData, dev_addr);
    return USBD_OK;
}

USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef *pdev, uint8_t ep_addr,
                                     uint8_t *pbuf, uint32_t size)
{
    (void)HAL_PCD_EP_Transmit(pdev->pData, ep_addr, pbuf, size);
    return USBD_OK;
}

USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef *pdev, uint8_t ep_addr,
                                          uint8_t *pbuf, uint32_t size)
{
    (void)HAL_PCD_EP_Receive(pdev->pData, ep_addr, pbuf, size);
    return USBD_OK;
}

uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    return HAL_PCD_EP_GetRxCount(pdev->pData, ep_addr);
}

void USBD_LL_Delay(uint32_t delay)
{
    HAL_Delay(delay);
}
