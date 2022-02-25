/**
 * Copyright (c) 2021 pmarques-dev @ github
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "opus_encoder.h"

// Declare Mutexs
mutex_t ENCODER_L_MTX;
mutex_t ENCODER_R_MTX;

void init_encoders()//Initialise 
{
    // Base pin to connect the A phase of the encoder.
    // The B phase must be connected to the next pin
    const uint PIN_AB_L = 18;
    const uint PIN_AB_R = 20;

    // Mutex inits
    mutex_init(&ENCODER_L_MTX);
    mutex_init(&ENCODER_R_MTX);


    PIO pio_L = PIO_LEFT;
    uint sm_L = PIO_SM_LEFT;

    uint program_offset = pio_add_program(pio_L, &quadrature_encoder_program);
    quadrature_encoder_program_init(pio_L, sm_L, program_offset, PIN_AB_L, 0);

    
    PIO pio_R = PIO_RIGHT;
    uint sm_R = PIO_SM_RIGHT;

    quadrature_encoder_program_init(pio_R, sm_R, program_offset, PIN_AB_R, 0);
}

encoder_t get_encoder_count(side_t side) // Side is LEFT_ENCODER or RIGHT_ENCODER
{
    encoder_t tick_time;
    tick_time.ticks = 0; // defaults to 0 if no response
    tick_time.time = nil_time;
    // tick_time.time = 1;
    if (side == LEFT)
    {
        mutex_enter_blocking(&ENCODER_L_MTX);
        tick_time.ticks = quadrature_encoder_get_count(PIO_LEFT,PIO_SM_LEFT);
        // tick_time.time = time_us_64();
        tick_time.time = get_absolute_time();
        mutex_exit(&ENCODER_L_MTX);
    }
    else if (side == RIGHT)
    {
        mutex_enter_blocking(&ENCODER_R_MTX);
        tick_time.ticks = quadrature_encoder_get_count(PIO_RIGHT,PIO_SM_RIGHT);
        tick_time.time = get_absolute_time();
        // tick_time.time = time_us_64();
        mutex_exit(&ENCODER_R_MTX);
    }
    else
    {
        // printf("WARNING: No support for get encoder count for side <%d>",side);
    }

    return tick_time;
}
