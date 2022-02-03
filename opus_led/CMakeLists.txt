add_library(opus_led opus_led.c)

# pull in common dependencies and additional pwm hardware support
target_link_libraries(opus_led pico_stdlib hardware_pwm hardware_sync)
target_include_directories(opus_led PUBLIC ${INCLUDE_DIR})
