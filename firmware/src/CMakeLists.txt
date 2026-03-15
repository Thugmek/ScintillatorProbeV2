target_sources(firmware PRIVATE
  main.cpp
  hal.cpp
  capture.cpp
  logging/log.cpp
  usb/usbd_conf.cpp
  usb/usbd_desc.cpp
  usb/usbd_cdc_if.cpp
  protocol/cobs.cpp
  protocol/protocol.cpp
)
target_include_directories(firmware PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(firmware PRIVATE stm32_hal freertos stm32_usb_device segger)

add_subdirectory(device)
