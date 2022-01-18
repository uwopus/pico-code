/**
 * Copyright (c) 2021 pmarques-dev @ github
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "opus_velocity.h"

// Declare Mutexs
mutex_t VEL_GOAL_L_MTX;
mutex_t VEL_GOAL_R_MTX;

// Declare Velocities That are shared with comms
float vel_goal_R;
float vel_goal_L;

// Controller Params Init
controller_t controller_params_L;
controller_t controller_params_R;

void init_velocity() // Initialise 
{
    // Mutex inits
    mutex_init(&VEL_GOAL_L_MTX);
    mutex_init(&VEL_GOAL_R_MTX);

    // Init starting velocity setpoints
    mutex_enter_blocking(&VEL_GOAL_L_MTX);
    vel_goal_L = 0;
    mutex_exit(&VEL_GOAL_L_MTX);
    
    
    mutex_enter_blocking(&VEL_GOAL_R_MTX);
    vel_goal_L = 0;
    mutex_exit(&VEL_GOAL_R_MTX);

    // Default Values for Controller for now
    controller_params_L.P = 1; // These need to be set somehow by the zero for easy prototyping
    controller_params_R.P = 1;

}


static float get_goal_velocity(side_t side_to_update) // Static update velocity function that should only exist in this file
{
    // Declare local versions of those velocities
    float vel_loc_goal; // Local velocity goal

    if (side_to_update == LEFT){
        mutex_enter_blocking(&VEL_GOAL_L_MTX);
        vel_loc_goal = vel_goal_L;
        mutex_exit(&VEL_GOAL_L_MTX);
    }
    else if (side_to_update == RIGHT){
        mutex_enter_blocking(&VEL_GOAL_R_MTX);
        vel_loc_goal = vel_goal_R;
        mutex_exit(&VEL_GOAL_R_MTX);
    }
    else{
        //printf("WARNING: Side not supported in update velocity function")
    }

    return vel_loc_goal;
}

float get_cur_vel(side_t cur_vel_side)
{
    float velocity = 0;
    encoder_t cur_encd;
    encoder_t nxt_encd;
    cur_encd = get_encoder_count(cur_vel_side);
    nxt_encd = get_encoder_count(cur_vel_side);

    // Calculate velocity
    int32_t delta_ticks = (cur_encd.ticks - nxt_encd.ticks);
    int64_t delta_time = absolute_time_diff_us(cur_encd.time,nxt_encd.time);
    float rotations = ((float) delta_ticks) * TICKS_TO_ROTATIONS;
    velocity = rotations * RADIUS * M_TWOPI / ((float)delta_time);

    return velocity;
}

float get_error(side_t error_side)
{
    float error = 0;
    error = get_goal_velocity(error_side) - get_cur_vel(error_side);
    return error;
}

controller_t get_controller_params(side_t controller_side){
    controller_t selected_controller;
    if (controller_side == LEFT)
    {
        selected_controller = controller_params_L;
    }
    else if (controller_side == RIGHT)
    {
        selected_controller = controller_params_R;
    }
    else{
        //printf("WARNING: Side not supported in get_controller_params function")
    }
    return selected_controller;
}

static inline float saturate(float value){
    if (value > 1.0){
        return 1.0;
    }
    else if (value < -1){
        return -1.0;
    }
    return 0.0;
}

static inline float map(float value){ // assuming input is from -1.0  to 1.0
    return value / 2 + 0.5;
}

float generate_set_duty(side_t duty_side) // This is the controller
{
    float duty = 0.5;
    float error = get_error(duty_side);

    // Get a raw fix value first
    float fix_amount = error * get_controller_params(duty_side).P;

    // Then need to saturate this value from -1 to 1
    fix_amount = saturate(fix_amount);

    // Then need to map to duty cycle which is from 0 to 1
    duty = map(fix_amount);

    return duty;
}
