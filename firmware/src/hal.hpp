#include "stm32h7xx_hal.h"

#define LCD_CS_Pin GPIO_PIN_4
#define LCD_CS_GPIO_Port GPIOA

namespace hal {
    void panic(void);
    void init(void);

    extern TIM_HandleTypeDef htim2;
    extern ADC_HandleTypeDef hadc1;
    extern ADC_HandleTypeDef hadc2;
    extern DMA_HandleTypeDef hdma_adc1;
    extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
}