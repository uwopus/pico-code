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

semaphore_t comm_pkt_available_sem;

extern mutex_t ENCODER_L_MTX;
extern mutex_t ENCODER_R_MTX;

#endif