/**
 * Copyright (c) 2021 pmarques-dev @ github
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "opus_encoder.h"

void init_encoders()//Initialise 
{
    // Base pin to connect the A phase of the encoder.
    // The B phase must be connected to the next pin
    const uint PIN_AB_L = 20;
    const uint PIN_AB_R = 18;

    // Mutex inits
    mutex_init(&ENCODER_L_MTX);
    mutex_init(&ENCODER_R_MTX);

    // stdio_init_all(); //Not sure if this should remain here or go else where

    PIO pio_L = PIO_LEFT;
    uint sm_L = PIO_SM_LEFT;

    uint program_offset = pio_add_program(pio_L, &quadrature_encoder_program);
    quadrature_encoder_program_init(pio_L, sm_L, program_offset, PIN_AB_L, 0);

    
    PIO pio_R = PIO_RIGHT;
    uint sm_R = PIO_SM_RIGHT;

    quadrature_encoder_program_init(pio_R, sm_R, program_offset, PIN_AB_R, 0);
}

int get_encoder_count(side_t side) // Side is LEFT_ENCODER or RIGHT_ENCODER
{
    int32_t count = 0; // defaults to 0 if no response
    if (side == LEFT)
    {
        mutex_enter_blocking(&ENCODER_L_MTX);
        count = quadrature_encoder_get_count(PIO_LEFT,PIO_SM_LEFT);
        mutex_exit(&ENCODER_L_MTX);
    }
    else if (side == RIGHT)
    {
        mutex_enter_blocking(&ENCODER_R_MTX);
        count = quadrature_encoder_get_count(PIO_RIGHT,PIO_SM_RIGHT);
        mutex_exit(&ENCODER_R_MTX);
    }
    else
    {
        // printf("WARNING: No support for get encoder count for side <%d>",side);
    }

    return count;
}
