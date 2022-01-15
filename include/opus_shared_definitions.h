/**
 * Copyright (c) 2022 Opus
 */

#ifndef OPUS_SHARED_DEFINITIONS_H
#define OPUS_SHARED_DEFINITIONS_H

#include "pico/sync.h"


typedef enum {
    LEFT = 0,
    RIGHT = 1
}side_t;

mutex_t ENCODER_L_MTX;
mutex_t ENCODER_R_MTX;

#endif