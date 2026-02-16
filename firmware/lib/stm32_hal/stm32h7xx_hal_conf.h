/**
 * @file  stm32h7xx_hal_conf.h
 * @brief HAL configuration – enable only the modules the project needs.
 *        Based on stm32h7xx_hal_conf_template.h from ST.
 */

#ifndef STM32H7xx_HAL_CONF_H
#define STM32H7xx_HAL_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* ── Module selection ──────────────────────────────────────────────── */
#define HAL_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_EXTI_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED
#define HAL_ADC_MODULE_ENABLED
#define HAL_SPI_MODULE_ENABLED

/* Uncomment to enable additional modules as needed:
#define HAL_CEC_MODULE_ENABLED
#define HAL_COMP_MODULE_ENABLED
#define HAL_CRC_MODULE_ENABLED
#define HAL_CRYP_MODULE_ENABLED
#define HAL_DAC_MODULE_ENABLED
#define HAL_DCMI_MODULE_ENABLED
#define HAL_DFSDM_MODULE_ENABLED
#define HAL_DMA2D_MODULE_ENABLED
#define HAL_DSI_MODULE_ENABLED
#define HAL_ETH_MODULE_ENABLED
#define HAL_FDCAN_MODULE_ENABLED
#define HAL_HASH_MODULE_ENABLED
#define HAL_HRTIM_MODULE_ENABLED
#define HAL_HSEM_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED
#define HAL_I2S_MODULE_ENABLED
#define HAL_IRDA_MODULE_ENABLED
#define HAL_IWDG_MODULE_ENABLED
#define HAL_JPEG_MODULE_ENABLED
#define HAL_LPTIM_MODULE_ENABLED
#define HAL_LTDC_MODULE_ENABLED
#define HAL_MDIOS_MODULE_ENABLED
#define HAL_MDMA_MODULE_ENABLED
#define HAL_MMC_MODULE_ENABLED
#define HAL_NAND_MODULE_ENABLED
#define HAL_NOR_MODULE_ENABLED
#define HAL_OPAMP_MODULE_ENABLED
#define HAL_OSPI_MODULE_ENABLED
#define HAL_QSPI_MODULE_ENABLED
#define HAL_RNG_MODULE_ENABLED
#define HAL_RTC_MODULE_ENABLED
#define HAL_SAI_MODULE_ENABLED
#define HAL_SD_MODULE_ENABLED
#define HAL_SDRAM_MODULE_ENABLED
#define HAL_SMARTCARD_MODULE_ENABLED
#define HAL_SMBUS_MODULE_ENABLED
#define HAL_SPDIFRX_MODULE_ENABLED
#define HAL_SRAM_MODULE_ENABLED
#define HAL_SWPMI_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED
#define HAL_USART_MODULE_ENABLED
#define HAL_WWDG_MODULE_ENABLED
*/

/* ── Oscillator values ─────────────────────────────────────────────── */
#if !defined(HSE_VALUE)
  #define HSE_VALUE    (25000000UL)
#endif

#if !defined(HSE_STARTUP_TIMEOUT)
  #define HSE_STARTUP_TIMEOUT    (100UL)
#endif

#if !defined(CSI_VALUE)
  #define CSI_VALUE    (4000000UL)
#endif

#if !defined(HSI_VALUE)
  #define HSI_VALUE    (64000000UL)
#endif

#if !defined(LSE_VALUE)
  #define LSE_VALUE    (32768UL)
#endif

#if !defined(LSE_STARTUP_TIMEOUT)
  #define LSE_STARTUP_TIMEOUT    (5000UL)
#endif

#if !defined(LSI_VALUE)
  #define LSI_VALUE    (32000UL)
#endif

#if !defined(EXTERNAL_CLOCK_VALUE)
  #define EXTERNAL_CLOCK_VALUE    12288000UL
#endif

/* ── System configuration ──────────────────────────────────────────── */
#define VDD_VALUE                    (3300UL)
#define TICK_INT_PRIORITY            (0x0FUL)
#define USE_RTOS                     0
#define USE_SD_TRANSCEIVER           0U
#define USE_SPI_CRC                  0U

/* ── Ethernet configuration ────────────────────────────────────────── */
#define ETH_TX_DESC_CNT              4U
#define ETH_RX_DESC_CNT              4U
#define ETH_MAC_ADDR0                (0x02UL)
#define ETH_MAC_ADDR1                (0x00UL)
#define ETH_MAC_ADDR2                (0x00UL)
#define ETH_MAC_ADDR3                (0x00UL)
#define ETH_MAC_ADDR4                (0x00UL)
#define ETH_MAC_ADDR5                (0x00UL)

/* ── Assert / debug ────────────────────────────────────────────────── */
/* #define USE_FULL_ASSERT  1U */

/* ── Module includes ───────────────────────────────────────────────── */
#ifdef HAL_RCC_MODULE_ENABLED
  #include "stm32h7xx_hal_rcc.h"
#endif

#ifdef HAL_GPIO_MODULE_ENABLED
  #include "stm32h7xx_hal_gpio.h"
#endif

#ifdef HAL_DMA_MODULE_ENABLED
  #include "stm32h7xx_hal_dma.h"
#endif

#ifdef HAL_EXTI_MODULE_ENABLED
  #include "stm32h7xx_hal_exti.h"
#endif

#ifdef HAL_CORTEX_MODULE_ENABLED
  #include "stm32h7xx_hal_cortex.h"
#endif

#ifdef HAL_FLASH_MODULE_ENABLED
  #include "stm32h7xx_hal_flash.h"
#endif

#ifdef HAL_PWR_MODULE_ENABLED
  #include "stm32h7xx_hal_pwr.h"
#endif

#ifdef HAL_ADC_MODULE_ENABLED
  #include "stm32h7xx_hal_adc.h"
#endif

#ifdef HAL_SPI_MODULE_ENABLED
  #include "stm32h7xx_hal_spi.h"
#endif

#ifdef HAL_TIM_MODULE_ENABLED
  #include "stm32h7xx_hal_tim.h"
#endif

/* ── assert_param macro ────────────────────────────────────────────── */
#ifdef USE_FULL_ASSERT
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t *file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif

#ifdef __cplusplus
}
#endif

#endif /* STM32H7xx_HAL_CONF_H */
