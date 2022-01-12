/**
 * Copyright (c) 2022 Opus
 */

#ifndef OPUS_ENCODER_H
#define OPUS_ENCODER_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/timer.h"

// This file includes the function to be used called quadrature_encoder_get_count(PIO pio, uint sm)
#include "opus_encoder.pio.h"

// Include side shared definitions
#include "opus_shared_definitions.h"


#define PIO_LEFT pio0
#define PIO_RIGHT pio1
#define PIO_SM_LEFT 0
#define PIO_SM_RIGHT 0

void init_encoders();//Initialise 

int get_encoder_count(side_t side); // Side is LEFT_ENCODER or RIGHT_ENCODER



#endif