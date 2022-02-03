add_library(opus_pwm opus_pwm.c)

# pull in common dependencies and additional pwm hardware support
target_link_libraries(opus_pwm pico_stdlib hardware_pwm hardware_clocks)
target_include_directories(opus_pwm PUBLIC ${INCLUDE_DIR})
