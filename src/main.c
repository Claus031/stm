#include "stm32f4xx.h"

static void delay_cycles(volatile uint32_t cycles)
{
    while (cycles-- > 0U)
    {
        __NOP();
    }
}

int main(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

    GPIOD->MODER &= ~(GPIO_MODER_MODE13_Msk);
    GPIOD->MODER |= (1U << GPIO_MODER_MODE13_Pos);
    GPIOD->OTYPER &= ~(GPIO_OTYPER_OT13);
    GPIOD->OSPEEDR |= (1U << GPIO_OSPEEDR_OSPEED13_Pos);
    GPIOD->PUPDR &= ~(GPIO_PUPDR_PUPD13_Msk);

    while (1)
    {
        GPIOD->BSRR = GPIO_BSRR_BS13;
        delay_cycles(800000U);

        GPIOD->BSRR = GPIO_BSRR_BR13;
        delay_cycles(800000U);
    }
}
