#include "hal.hpp"
#include "stm32h7xx_hal.h"

namespace hal {
    void init(void) {
        HAL_Init();
    }
}

extern "C" void SysTick_Handler(void) {
    HAL_IncTick();
}
