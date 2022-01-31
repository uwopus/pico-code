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
    // uint64_t time; // time in absolute time units
    absolute_time_t time; // time in absolute time units
}encoder_t;


typedef enum config{
    SET_P_L = 0,
    SET_I_L = 1,
    SET_D_L = 2,
    SET_N_L = 3,
    SET_P_R = 4,
    SET_I_R = 5,
    SET_D_R = 6,
    SET_N_R = 7
}config_t;

typedef struct velocity_controller_parms{
    uint8_t P;
    uint8_t I;
    uint8_t D;
    uint8_t N;
}controller_t;

typedef enum {
    GO = 0,
    STOP = 1
}picoState_t;


extern picoState_t pico_State;
extern mutex_t PICO_STATE_MTX;

extern semaphore_t sem_spi_rx;

extern mutex_t ENCODER_L_MTX;
extern mutex_t ENCODER_R_MTX;

extern mutex_t VEL_GOAL_L_MTX;
extern mutex_t VEL_GOAL_R_MTX;

extern float vel_goal_L; // 
extern float vel_goal_R;

extern mutex_t controller_params_L_mtx; 
extern mutex_t controller_params_R_mtx;

extern controller_t controller_params_L;
extern controller_t controller_params_R;


#endif