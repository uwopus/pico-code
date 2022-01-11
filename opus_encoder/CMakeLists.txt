add_library(opus_encoder opus_encoder.c)

pico_generate_pio_header(opus_encoder ${CMAKE_CURRENT_LIST_DIR}/opus_encoder.pio)

target_link_libraries(opus_encoder PUBLIC
        pico_stdlib
        pico_multicore
        hardware_pio
        )

target_include_directories(opus_encoder PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})