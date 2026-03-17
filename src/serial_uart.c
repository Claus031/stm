#include "serial_uart.h"

#include "stm32f4xx_hal.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

USBD_HandleTypeDef husbd;

static void system_clock_config(void)
{
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* HSE 8 MHz -> PLL -> 168 MHz SYSCLK, 48 MHz USB (PLLQ=7) */
    osc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    osc.HSEState       = RCC_HSE_ON;
    osc.PLL.PLLState   = RCC_PLL_ON;
    osc.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    osc.PLL.PLLM       = 8U;
    osc.PLL.PLLN       = 336U;
    osc.PLL.PLLP       = RCC_PLLP_DIV2;
    osc.PLL.PLLQ       = 7U;
    (void)HAL_RCC_OscConfig(&osc);

    clk.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                         RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clk.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    clk.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV4;
    clk.APB2CLKDivider = RCC_HCLK_DIV2;
    (void)HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_5);
}

void serial_uart_init(void)
{
    HAL_Init();
    system_clock_config();

    USBD_Init(&husbd, &CDC_Desc, 0U);
    USBD_RegisterClass(&husbd, &USBD_CDC);
    USBD_CDC_RegisterInterface(&husbd, &USBD_CDC_fops);
    USBD_Start(&husbd);
}

void serial_uart_write_byte(char byte)
{
    serial_uart_write(&byte, 1U);
}

void serial_uart_write(const char *buffer, size_t length)
{
    size_t i = 0U;

    while (i < length)
    {
        size_t j = i;

        while (j < length && buffer[j] != '\n')
        {
            j++;
        }

        if (j > i)
        {
            USB_CDC_Transmit((uint8_t *)&buffer[i], (uint16_t)(j - i));
        }

        if (j < length && buffer[j] == '\n')
        {
            USB_CDC_Transmit((uint8_t *)"\r\n", 2U);
            j++;
        }

        i = j;
    }
}
