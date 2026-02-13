cmake_minimum_required(VERSION 3.22)

include(cmake/Utilities.cmake)

project(
  ScintillatorProbe
  LANGUAGES C CXX ASM
  )

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# MCU option - add new chips here as needed
set(MCU
    "STM32H753ZI"
    CACHE STRING "Target MCU"
    )
set_property(CACHE MCU PROPERTY STRINGS STM32H753ZI)

# MCU-specific compiler flags
if(MCU MATCHES "STM32H7")
  set(MCU_FLAGS -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-d16)
else()
  message(FATAL_ERROR "Unknown MCU family for ${MCU}")
endif()

add_compile_options(-mthumb ${MCU_FLAGS})
add_compile_options(
  -Wall
  -Wextra
  -ffunction-sections
  -fdata-sections
  -fno-exceptions
  $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
  )

# Debug: include dwarf info, no optimization (good for stepping)
add_compile_options($<$<CONFIG:Debug>:-Og> $<$<CONFIG:Debug>:-g3> $<$<CONFIG:Debug>:-gdwarf-4>)
add_compile_options($<$<CONFIG:Release>:-Os>)

add_link_options(-mthumb ${MCU_FLAGS})
add_link_options(-Wl,--gc-sections -Wl,--print-memory-usage)

# Main firmware executable
add_executable(firmware)

add_subdirectory(lib/stm32_hal)
add_subdirectory(src)

# Post-build: generate .bin, .hex, report size
objcopy(firmware ihex .hex)
objcopy(firmware binary .bin)
report_size(firmware)
