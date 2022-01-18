/**
 * Copyright (c) 2022 Opus
 */

#ifndef OPUS_SHARED_DEFINITIONS_H
#define OPUS_SHARED_DEFINITIONS_H

#include "pico/sync.h"
#include "pico/time.h"


typedef enum {
    LEFT = 0,
    RIGHT = 1
}side_t;

typedef struct encoder_ticks_and_time{
    int32_t ticks;
    absolute_time_t time; // time in absolute time units
}encoder_t;

typedef struct velocity_controller_parms{
    uint8_t P;
    uint8_t I;
    uint8_t D;
}controller_t;

semaphore_t comm_pkt_available_sem;

extern mutex_t ENCODER_L_MTX;
extern mutex_t ENCODER_R_MTX;

extern mutex_t VEL_GOAL_L_MTX;
extern mutex_t VEL_GOAL_R_MTX;

extern float vel_goal_L;
extern float vel_goal_R;

extern controller_t controller_params_L;
extern controller_t controller_params_R;

#endif