add_library(segger STATIC
  SEGGER_RTT.c
  SEGGER_RTT_printf.c
  SEGGER_RTT_ASM_ARMv7M.S
)

target_include_directories(segger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Suppress warnings from vendor code
target_compile_options(segger PRIVATE -Wno-unused-parameter -Wno-sign-conversion)
