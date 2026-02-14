target_sources(firmware PRIVATE main.cpp hal.cpp)
target_include_directories(firmware PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(firmware PRIVATE stm32_hal segger)

add_subdirectory(device)
