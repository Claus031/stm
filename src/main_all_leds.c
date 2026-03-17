#include "stm32f4xx.h"

#include <stdio.h>

#include "serial_uart.h"

static void delay_cycles(volatile uint32_t cycles)
{
    while (cycles-- > 0U)
    {
        __NOP();
    }
}

static void init_led_gpio(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

    GPIOD->MODER &= ~(GPIO_MODER_MODE12_Msk | GPIO_MODER_MODE13_Msk | GPIO_MODER_MODE14_Msk | GPIO_MODER_MODE15_Msk);
    GPIOD->MODER |= (1U << GPIO_MODER_MODE12_Pos) |
                    (1U << GPIO_MODER_MODE13_Pos) |
                    (1U << GPIO_MODER_MODE14_Pos) |
                    (1U << GPIO_MODER_MODE15_Pos);

    GPIOD->OTYPER &= ~(GPIO_OTYPER_OT12 | GPIO_OTYPER_OT13 | GPIO_OTYPER_OT14 | GPIO_OTYPER_OT15);

    GPIOD->OSPEEDR |= (1U << GPIO_OSPEEDR_OSPEED12_Pos) |
                      (1U << GPIO_OSPEEDR_OSPEED13_Pos) |
                      (1U << GPIO_OSPEEDR_OSPEED14_Pos) |
                      (1U << GPIO_OSPEEDR_OSPEED15_Pos);

    GPIOD->PUPDR &= ~(GPIO_PUPDR_PUPD12_Msk | GPIO_PUPDR_PUPD13_Msk | GPIO_PUPDR_PUPD14_Msk | GPIO_PUPDR_PUPD15_Msk);
}

int main(void)
{
    const uint32_t led_mask = GPIO_BSRR_BS12 | GPIO_BSRR_BS13 | GPIO_BSRR_BS14 | GPIO_BSRR_BS15;
    const uint32_t sequence[] = {
        GPIO_BSRR_BS12,
        GPIO_BSRR_BS13,
        GPIO_BSRR_BS14,
        GPIO_BSRR_BS15,
    };
    const char *colors[] = {
        "GREEN",
        "ORANGE",
        "RED",
        "BLUE",
    };
    uint32_t index = 0U;

    init_led_gpio();
    serial_uart_init();
    printf("STM32 LED sequence started\n");

    while (1)
    {
        GPIOD->BSRR = (led_mask << 16U);
        GPIOD->BSRR = sequence[index];
        printf("LED ON: %s\n", colors[index]);
        delay_cycles(13000000U);

        index++;
        if (index >= (sizeof(sequence) / sizeof(sequence[0])))
        {
            index = 0U;
        }
    }
}
