add_library(opus_pwm "")

# pull in common dependencies and additional pwm hardware support
target_link_libraries(opus_pwm pico_stdlib hardware_pwm)
