include(FetchContent)

FetchContent_Declare(
  stm32_mw_usb_device
  GIT_REPOSITORY https://github.com/STMicroelectronics/stm32_mw_usb_device.git
  GIT_TAG        v2.11.3
  GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(stm32_mw_usb_device)

file(GLOB USB_CORE_SOURCES "${stm32_mw_usb_device_SOURCE_DIR}/Core/Src/*.c")
list(FILTER USB_CORE_SOURCES EXCLUDE REGEX "_template\\.c$")
set(USB_CDC_SOURCE "${stm32_mw_usb_device_SOURCE_DIR}/Class/CDC/Src/usbd_cdc.c")

add_library(stm32_usb_device STATIC ${USB_CORE_SOURCES} ${USB_CDC_SOURCE})

target_include_directories(stm32_usb_device PUBLIC
  ${stm32_mw_usb_device_SOURCE_DIR}/Core/Inc
  ${stm32_mw_usb_device_SOURCE_DIR}/Class/CDC/Inc
  ${CMAKE_SOURCE_DIR}/src/usb
)

target_link_libraries(stm32_usb_device PUBLIC stm32_hal)

target_compile_options(stm32_usb_device PRIVATE -Wno-unused-parameter)
