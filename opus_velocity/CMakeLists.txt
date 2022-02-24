add_library(opus_velocity opus_velocity.c)

target_include_directories(opus_velocity PUBLIC ${INCLUDE_DIR})

target_link_libraries(opus_velocity PUBLIC
        pico_stdlib
        pico_multicore
        hardware_pio
        pico_time
        opus_encoder
        pico_float
        opus_pwm
        )

