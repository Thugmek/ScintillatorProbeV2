include(FetchContent)

# ---------- ST HAL driver ----------
FetchContent_Declare(
  stm32h7_hal_driver
  GIT_REPOSITORY https://github.com/STMicroelectronics/stm32h7xx_hal_driver.git
  GIT_TAG        v1.11.5
  GIT_SHALLOW    TRUE
)

# ---------- CMSIS Device H7 ----------
FetchContent_Declare(
  cmsis_device_h7
  GIT_REPOSITORY https://github.com/STMicroelectronics/cmsis_device_h7.git
  GIT_TAG        v1.10.6
  GIT_SHALLOW    TRUE
)

# ---------- CMSIS Core ----------
FetchContent_Declare(
  cmsis_core
  GIT_REPOSITORY https://github.com/STMicroelectronics/cmsis_core.git
  GIT_TAG        v5.9.0
  GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(stm32h7_hal_driver cmsis_device_h7 cmsis_core)

# Collect HAL driver sources, excluding template files
file(GLOB HAL_SOURCES "${stm32h7_hal_driver_SOURCE_DIR}/Src/*.c")
list(FILTER HAL_SOURCES EXCLUDE REGEX "_template\\.c$")

# CMSIS system init source
set(SYSTEM_SOURCE "${cmsis_device_h7_SOURCE_DIR}/Source/Templates/system_stm32h7xx.c")

# Build static library
add_library(stm32_hal STATIC ${HAL_SOURCES} ${SYSTEM_SOURCE})

target_include_directories(stm32_hal PUBLIC
  ${cmsis_core_SOURCE_DIR}/Core/Include
  ${cmsis_device_h7_SOURCE_DIR}/Include
  ${stm32h7_hal_driver_SOURCE_DIR}/Inc
  ${CMAKE_CURRENT_SOURCE_DIR}  # for stm32h7xx_hal_conf.h
)

target_compile_definitions(stm32_hal PUBLIC
  STM32H753xx
  USE_HAL_DRIVER
)

# Suppress warnings from vendor code
target_compile_options(stm32_hal PRIVATE -Wno-unused-parameter)
