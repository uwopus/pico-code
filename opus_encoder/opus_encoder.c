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

    stdio_init_all(); //Not sure if this should remain here or go else where

    PIO pio_L = PIO_LEFT;
    uint sm_L = PIO_SM_LEFT;

    uint offset_L = pio_add_program(pio_L, &quadrature_encoder_program);
    quadrature_encoder_program_init(pio_L, sm_L, offset_L, PIN_AB_L, 0);

    
    PIO pio_R = PIO_RIGHT;
    uint sm_R = PIO_SM_RIGHT;

    uint offset_R = pio_add_program(pio_R, &quadrature_encoder_program);
    quadrature_encoder_program_init(pio_R, sm_R, offset_R, PIN_AB_R, 0);
}

int get_encoder_count(side_t side) // Side is LEFT_ENCODER or RIGHT_ENCODER
{
    int32_t count = 0; // defaults to 0 if no response
    if (side == LEFT)
    {
        count = quadrature_encoder_get_count(PIO_LEFT,PIO_SM_LEFT);
    }
    else if (side == RIGHT)
    {
        count = quadrature_encoder_get_count(PIO_RIGHT,PIO_SM_RIGHT);
    }
    else
    {
        // printf("WARNING: No support for get encoder count for side <%d>",side);
    }

    return count;
}
