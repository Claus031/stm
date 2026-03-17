#ifndef STM32F4XX_HAL_CONF_H
#define STM32F4XX_HAL_CONF_H

#define HAL_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_PCD_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED

#define HSE_VALUE             8000000U
#define HSE_STARTUP_TIMEOUT   100U
#define HSI_VALUE             16000000U
#define LSI_VALUE             32000U
#define LSE_VALUE             32768U
#define LSE_STARTUP_TIMEOUT   5000U

#define VDD_VALUE             3300U
#define TICK_INT_PRIORITY     0x0FU
#define USE_RTOS              0U
#define EXTERNAL_CLOCK_VALUE  12288000U

#include "stm32f4xx.h"

#ifdef HAL_RCC_MODULE_ENABLED
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_rcc_ex.h"
#endif

#ifdef HAL_GPIO_MODULE_ENABLED
#include "stm32f4xx_hal_gpio.h"
#endif

#ifdef HAL_FLASH_MODULE_ENABLED
#include "stm32f4xx_hal_flash.h"
#include "stm32f4xx_hal_flash_ex.h"
#include "stm32f4xx_hal_flash_ramfunc.h"
#endif

#ifdef HAL_PWR_MODULE_ENABLED
#include "stm32f4xx_hal_pwr.h"
#include "stm32f4xx_hal_pwr_ex.h"
#endif

#ifdef HAL_CORTEX_MODULE_ENABLED
#include "stm32f4xx_hal_cortex.h"
#endif

#ifdef HAL_PCD_MODULE_ENABLED
#include "stm32f4xx_hal_pcd.h"
#include "stm32f4xx_hal_pcd_ex.h"
#endif

#define assert_param(expr) ((void)0U)

#endif
