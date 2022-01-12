add_library(opus_comms opus_comms.c)

# pull in common dependencies and additional pwm hardware support
target_link_libraries(opus_comms pico_stdlib hardware_spi)
target_include_directories(opus_comms PUBLIC ${INCLUDE_DIR})
