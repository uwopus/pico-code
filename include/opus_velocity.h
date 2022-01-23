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

// Include side shared definitions
#include "opus_shared_definitions.h"

#define TICKS_PER_ROTATION 1992.6
#define RADIUS 0.05 // in m, maybe this should be passed in by the zero
#define GEAR_RATIO (10.f/80.f) // 
#define ENC_HIST_BUFF_LEN 2 

void init_velocity(); // Initialise

float get_cur_vel(side_t cur_vel_side);

float get_error(side_t error_side);

float get_controller_pwm(side_t error_side);

float generate_set_duty(side_t duty_side); // This is the controller

controller_t get_controller_params(side_t controller_side);

#endif