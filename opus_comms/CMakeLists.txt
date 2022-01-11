add_library(opus_comms opus_comms.c opus_comms.h)

# pull in common dependencies and additional pwm hardware support
target_link_libraries(opus_comms pico_stdlib hardware_spi)
target_include_directories(opus_comms PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
