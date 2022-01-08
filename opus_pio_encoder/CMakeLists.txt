add_executable(opus_pio_encoder)

pico_generate_pio_header(opus_pio_encoder ${CMAKE_CURRENT_LIST_DIR}/opus_pio_encoder.pio)

target_sources(opus_pio_encoder PRIVATE opus_pio_encoder.cpp)

target_link_libraries(opus_pio_encoder PRIVATE
        pico_stdlib
        hardware_pio
        )

        
# enable usb output, disable uart output
pico_enable_stdio_usb(opus_pio_encoder 1)
pico_enable_stdio_uart(opus_pio_encoder 0)

pico_add_extra_outputs(opus_pio_encoder)
