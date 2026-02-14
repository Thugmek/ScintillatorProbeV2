#include "stm32h7xx_hal.h"

namespace hal {
    void panic(void);
    void init(void);

    extern TIM_HandleTypeDef htim1;
    extern ADC_HandleTypeDef hadc1;
    extern ADC_HandleTypeDef hadc2;
    extern DMA_HandleTypeDef hdma_adc1;
}