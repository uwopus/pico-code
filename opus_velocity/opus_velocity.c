/**
 * Copyright (c) 2021 pmarques-dev @ github
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "opus_velocity.h"

// Declare Mutexs
mutex_t VEL_GOAL_L_MTX;
mutex_t VEL_GOAL_R_MTX;

mutex_t ENCD_HIST_MTX;

// Declare Velocities That are shared with comms
float vel_goal_L; // Linear velocity m/s
float vel_goal_R; // Linear velocity m/s

// Controller Params Init
controller_t controller_params_L;
controller_t controller_params_R;

encoder_t encoder_hist_L[ENC_HIST_BUFF_LEN];
encoder_t encoder_hist_R[ENC_HIST_BUFF_LEN];

uint8_t hist_indx;

repeating_timer_t encoder_hist_timer;

bool update_encd_hist(repeating_timer_t *t_val);

void init_velocity() // Initialise 
{
    // Mutex inits
    mutex_init(&VEL_GOAL_L_MTX);
    mutex_init(&VEL_GOAL_R_MTX);

    mutex_init(&ENCD_HIST_MTX);

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


    // Init timers
    add_repeating_timer_ms(100,update_encd_hist,NULL,&encoder_hist_timer);


    // Init buffer
    mutex_enter_blocking(&ENCD_HIST_MTX);
    for (int i = ENC_HIST_BUFF_LEN - 1; i > 0; --i){
        encoder_hist_L[i].ticks = 0;
        encoder_hist_R[i].ticks = 0;
        encoder_hist_L[i].time = get_absolute_time();
        encoder_hist_R[i].time = get_absolute_time();
    }
    hist_indx = 0;
    mutex_exit(&ENCD_HIST_MTX);
}

bool update_encd_hist(repeating_timer_t *t_val){
    //for printing
    encoder_t encoder_l_hist_val;
    encoder_t encoder_r_hist_val;

    printf("time: %llu\r\n",time_us_64());
    mutex_enter_blocking(&ENCD_HIST_MTX);
    encoder_l_hist_val =  get_encoder_count(LEFT);
    encoder_r_hist_val =  get_encoder_count(RIGHT);
    encoder_hist_L[hist_indx] = encoder_l_hist_val;
    encoder_hist_R[hist_indx] = encoder_r_hist_val;
    hist_indx = (hist_indx + 1) % ENC_HIST_BUFF_LEN;
    mutex_exit(&ENCD_HIST_MTX);
    printf("updated vals: encd_l.tick: %d | encd_l.time: %llu | endc_r.tick: %d | encd_r.time: %llu\r\n",encoder_l_hist_val.ticks,encoder_l_hist_val.time,encoder_r_hist_val.ticks,encoder_r_hist_val.time);
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
    if (cur_vel_side == LEFT){
        mutex_enter_blocking(&ENCD_HIST_MTX);
        nxt_encd = encoder_hist_L[hist_indx];
        cur_encd = encoder_hist_L[(hist_indx + 1) % ENC_HIST_BUFF_LEN];
        mutex_exit(&ENCD_HIST_MTX);
    }
    else if (cur_vel_side == RIGHT){
        mutex_enter_blocking(&ENCD_HIST_MTX);
        nxt_encd = encoder_hist_R[hist_indx];
        cur_encd = encoder_hist_R[(hist_indx + 1) % ENC_HIST_BUFF_LEN];
        mutex_exit(&ENCD_HIST_MTX);
    }
    else{
        //printf("WARNING: Side not supported in get cur velocity function")
    }

    // Calculate velocity
    int32_t delta_ticks = (nxt_encd.ticks - cur_encd.ticks);
    int64_t delta_time = absolute_time_diff_us(cur_encd.time,nxt_encd.time);

    // printf("cur_encd_time: %llu | nxt_encd_time: %llu\n\r",cur_encd.time,nxt_encd.time);
    
    float rotations = ((float) delta_ticks) / TICKS_PER_ROTATION;
    velocity = (rotations * GEAR_RATIO * RADIUS * M_TWOPI) / ((float)(delta_time * 1E-6));
    printf("Current Velocity: %5.2f [m/s]\n\r",velocity);


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
    return value;
}

static inline float map(float value){ // map from -1 - 1 -> 0.1 - 0.2
    return value / 20 + 0.15;
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
