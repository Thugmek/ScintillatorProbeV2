#include "stm32h7xx_hal.h"

#define LCD_CS_Pin GPIO_PIN_4
#define LCD_CS_GPIO_Port GPIOA

#define LCD_DC_Pin GPIO_PIN_5
#define LCD_DC_GPIO_Port GPIOA

#define LCD_RST_Pin GPIO_PIN_3
#define LCD_RST_GPIO_Port GPIOA

namespace hal {
    void panic(void);
    void init(void);

    extern TIM_HandleTypeDef htim1;
    extern ADC_HandleTypeDef hadc1;
    extern ADC_HandleTypeDef hadc2;
    extern DMA_HandleTypeDef hdma_adc1;
    extern SPI_HandleTypeDef hspi1;
}