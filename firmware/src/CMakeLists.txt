target_sources(firmware PRIVATE main.cpp hal.cpp capture.cpp logging/log.cpp lcd/ili9488.cpp)
target_include_directories(firmware PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(firmware PRIVATE stm32_hal segger)

# --- Sprite generation from PNG images ---
set(SPRITE_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
set(PNG2SPRITE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/png2sprite.py)

file(GLOB SPRITE_PNGS ${CMAKE_CURRENT_SOURCE_DIR}/resources/images/*.png)

foreach(PNG_FILE ${SPRITE_PNGS})
    get_filename_component(SPRITE_NAME ${PNG_FILE} NAME_WE)
    set(SPRITE_HPP ${SPRITE_GEN_DIR}/${SPRITE_NAME}.hpp)
    add_custom_command(
        OUTPUT ${SPRITE_HPP}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SPRITE_GEN_DIR}
        COMMAND python3 ${PNG2SPRITE} ${PNG_FILE} ${SPRITE_HPP}
        DEPENDS ${PNG_FILE} ${PNG2SPRITE}
        COMMENT "Generating sprite header for ${SPRITE_NAME}"
    )
    list(APPEND GENERATED_SPRITE_HEADERS ${SPRITE_HPP})
endforeach()

if(GENERATED_SPRITE_HEADERS)
    add_custom_target(generate_sprites DEPENDS ${GENERATED_SPRITE_HEADERS})
    add_dependencies(firmware generate_sprites)
    target_include_directories(firmware PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
endif()

add_subdirectory(device)
