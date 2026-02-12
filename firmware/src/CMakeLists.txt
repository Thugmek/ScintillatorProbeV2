target_sources(firmware PRIVATE main.cpp)
target_include_directories(firmware PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(device)
