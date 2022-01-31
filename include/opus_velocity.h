/**
 * Copyright (c) 2022 Opus
 */

#ifndef OPUS_VELOCITY_H
#define OPUS_VELOCITY_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "pico/time.h"
#include "math.h"
#include "pico/float.h"


#include "opus_encoder.h"
#include "opus_pwm.h"

// Include side shared definitions
#include "opus_shared_definitions.h"

#define TICKS_PER_ROTATION 1992.6
#define RADIUS 0.05 // in m, maybe this should be passed in by the zero
#define GEAR_RATIO (10.f/80.f) // 
#define ENC_HIST_BUFF_LEN 2 
#define ENC_SAMPLE_TIME 20 // micro seconds
#define VEL_SAMPLE_TIME 100 // needs to be implemented and checked

// Motor controller default values
#define DEFAULT_L_CONTROLLER_P 1
#define DEFAULT_L_CONTROLLER_I 1
#define DEFAULT_L_CONTROLLER_D 1
#define DEFAULT_L_CONTROLLER_N 20 // from site example https://www.scilab.org/discrete-time-pid-controller-implementation
#define DEFAULT_R_CONTROLLER_P 1
#define DEFAULT_R_CONTROLLER_I 1
#define DEFAULT_R_CONTROLLER_D 1
#define DEFAULT_R_CONTROLLER_N 20
#define STOP_DUTY_CYCLE 1.5f

void init_velocity(); // Initialise

float get_cur_vel(side_t cur_vel_side);

float get_error(side_t error_side);

float get_controller_pwm(side_t error_side);

float generate_set_duty(side_t duty_side); // This is the controller

void hard_stop_motors();

controller_t get_controller_params(side_t controller_side);

#endif