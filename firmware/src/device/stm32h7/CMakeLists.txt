target_sources(firmware PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/startup/stm32h753xx.s)

if(MCU STREQUAL "STM32H753ZI")
  target_set_linker_script(firmware "${CMAKE_CURRENT_SOURCE_DIR}/linker/stm32h753zi.ld")
else()
  message(FATAL_ERROR "Unknown linker script for ${MCU}")
endif()

target_compile_definitions(firmware PRIVATE STM32H753xx)
