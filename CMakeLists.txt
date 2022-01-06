cmake_minimum_required(VERSION 3.12)

# Change your executable name to something creative!
set(NAME pico_code) # <-- Name your project/executable here!

include(pico_sdk_import.cmake)

# Gooey boilerplate
project(${NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

# Add your source files
add_executable(${NAME}
    main.c # <-- Add source files here!
)

# include_directories(opus_pwm)
add_subdirectory(opus_pwm)
add_subdirectory(opus_motor_controller_emulator)
# Include required libraries
# include(opus_pwm/opus_pwm)

# create map/bin/hex file etc.
pico_add_extra_outputs(${NAME})

# Don't forget to link the libraries you need!
target_link_libraries(${NAME}
opus_pwm
opus_motor_controller_emulator
hardware_adc
)
