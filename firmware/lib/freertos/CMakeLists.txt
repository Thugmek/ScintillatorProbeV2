include(FetchContent)

# Provide the config target that FreeRTOS-Kernel's CMakeLists.txt expects
add_library(freertos_config INTERFACE)
target_include_directories(freertos_config SYSTEM INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}"   # for FreeRTOSConfig.h
)
# FreeRTOS config needs CMSIS / HAL headers for __NVIC_PRIO_BITS, stm32h7xx_hal.h
target_link_libraries(freertos_config INTERFACE stm32_hal)

# Select the Cortex-M7 port (r0p1 handles lazy FPU context saving)
set(FREERTOS_PORT "GCC_ARM_CM7" CACHE STRING "" FORCE)
set(FREERTOS_HEAP "4" CACHE STRING "" FORCE)

# ---------- FreeRTOS Kernel ----------
FetchContent_Declare(
  freertos_kernel
  GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
  GIT_TAG        V11.1.0
  GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(freertos_kernel)

# Convenience alias so the rest of the project just links "freertos"
add_library(freertos ALIAS freertos_kernel)
