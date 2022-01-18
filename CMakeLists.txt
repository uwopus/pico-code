cmake_minimum_required(VERSION 3.12)

# Change your executable name to something creative!
set(NAME pico_code) # <-- Name your project/executable here!

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

include(pico_sdk_import.cmake)

# Gooey boilerplate
project(${NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Add -O0 to remove optimizations when using gcc
# IF(CMAKE_COMPILER_IS_GNUCC)
#     set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
#     set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
# ENDIF(CMAKE_COMPILER_IS_GNUCC)

# Initialize the SDK
pico_sdk_init()

# Add your source files
add_executable(${NAME}
    main.c # <-- Add source files here!
)

target_include_directories(pico_code PUBLIC ${INCLUDE_DIR})

# include_directories(opus_pwm)
add_subdirectory(opus_pwm)
add_subdirectory(opus_comms)
add_subdirectory(opus_encoder)
# Include required libraries
# This assumes `pimoroni-pico` is stored alongside your project
# include(opus_pwm/opus_pwm)



# enable usb output, disable uart output
pico_enable_stdio_usb(pico_code 1)
pico_enable_stdio_uart(pico_code 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(${NAME})

# Don't forget to link the libraries you need!
target_link_libraries(${NAME}
opus_pwm
opus_comms
opus_encoder
)


