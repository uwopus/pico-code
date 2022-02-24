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

mutex_t controller_params_L_mtx;
mutex_t controller_params_R_mtx;

mutex_t PICO_STATE_MTX;
picoState_t pico_State;
// Controller Params Init
controller_t controller_params_L;
controller_t controller_params_R;

encoder_t encoder_hist_L[ENC_HIST_BUFF_LEN];
encoder_t encoder_hist_R[ENC_HIST_BUFF_LEN];

uint8_t hist_indx;

repeating_timer_t encoder_hist_timer;
repeating_timer_t vel_control_timer;

bool update_encd_hist(repeating_timer_t *t_val);
bool update_velocity_pwm(repeating_timer_t *t_val);

void init_velocity() // Initialise 
{
    // Mutex inits
    mutex_init(&VEL_GOAL_L_MTX);
    mutex_init(&VEL_GOAL_R_MTX);

    mutex_init(&ENCD_HIST_MTX);

    mutex_init(&controller_params_L_mtx);
    mutex_init(&controller_params_R_mtx);

    mutex_init(&PICO_STATE_MTX);


    // Init starting velocity setpoints
    mutex_enter_blocking(&VEL_GOAL_L_MTX);
    vel_goal_L = 0;
    mutex_exit(&VEL_GOAL_L_MTX);
    
    
    mutex_enter_blocking(&VEL_GOAL_R_MTX);
    vel_goal_L = 0;
    mutex_exit(&VEL_GOAL_R_MTX);

    // Default Values for Controller for now
    mutex_enter_blocking(&controller_params_L_mtx);
    controller_params_L.P = DEFAULT_L_CONTROLLER_P; // These need to be set somehow by the zero for easy prototyping
    controller_params_L.I = DEFAULT_L_CONTROLLER_I;
    controller_params_L.D = DEFAULT_L_CONTROLLER_D;
    controller_params_L.N = DEFAULT_L_CONTROLLER_N;
    mutex_exit(&controller_params_L_mtx);
    
    mutex_enter_blocking(&controller_params_R_mtx);
    controller_params_R.P = DEFAULT_R_CONTROLLER_P; // These need to be set somehow by the zero for easy prototyping
    controller_params_R.I = DEFAULT_R_CONTROLLER_I;
    controller_params_R.D = DEFAULT_R_CONTROLLER_D;
    controller_params_R.N = DEFAULT_R_CONTROLLER_N;
    mutex_exit(&controller_params_R_mtx);

    // Init pico state - this should start as stop until told to go from comms but for now keep it here, and maybe comms should start this
    pico_State = STOP_STATE;


    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    gpio_init(17);
    gpio_set_dir(17, GPIO_OUT);

    // Init timers
    add_repeating_timer_ms(ENC_SAMPLE_TIME,update_encd_hist,NULL,&encoder_hist_timer);
    add_repeating_timer_ms(VEL_SAMPLE_TIME,update_velocity_pwm,NULL,&vel_control_timer);


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

void hard_stop_motors(){
    set_pwm(LEFT,STOP_DUTY_CYCLE);
    set_pwm(RIGHT,STOP_DUTY_CYCLE);
}

bool update_velocity_pwm(repeating_timer_t *t_val){
    picoState_t cur_state = pico_State;

    if (cur_state == STOP_STATE){
        gpio_xor_mask(1 << 17);
        hard_stop_motors();
    }
    else if (cur_state == GO_STATE){
        gpio_put(17, 1);
        float duty_L = generate_set_duty(LEFT);
        set_pwm(LEFT,duty_L);
        float duty_R = generate_set_duty(RIGHT);
        set_pwm(RIGHT,duty_R);
    }

    return true; // important to return true to keep timer going

}


bool update_encd_hist(repeating_timer_t *t_val){
    //for printing
    encoder_t encoder_l_hist_val;
    encoder_t encoder_r_hist_val;
    gpio_xor_mask(1 << 25);



    // printf("time: %llu\r\n",time_us_64());
    mutex_enter_blocking(&ENCD_HIST_MTX);
    encoder_l_hist_val =  get_encoder_count(LEFT);
    encoder_r_hist_val =  get_encoder_count(RIGHT);
    encoder_hist_L[hist_indx] = encoder_l_hist_val;
    encoder_hist_R[hist_indx] = encoder_r_hist_val;
    hist_indx = (hist_indx + 1) % ENC_HIST_BUFF_LEN;
    mutex_exit(&ENCD_HIST_MTX);
    // printf("updated vals: encd_l.tick: %d | encd_l.time: %llu | endc_r.tick: %d | encd_r.time: %llu\r\n",encoder_l_hist_val.ticks,encoder_l_hist_val.time,encoder_r_hist_val.ticks,encoder_r_hist_val.time);

    return true;
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
    // printf("Rotations/second: %5.7f",rotations/(delta_time * 1E-6));
    velocity = (rotations * GEAR_RATIO * RADIUS * M_TWOPI) / ((float)(delta_time * 1E-6));
    // printf("Current Velocity: %5.7f [m/s]\n\r",velocity);


    return velocity;
}

float get_error(side_t error_side)
{
    float error = 0;
    float goal_vel = get_goal_velocity(error_side);
    float cur_vel = get_cur_vel(error_side);
    error = get_goal_velocity(error_side) - get_cur_vel(error_side);
    printf("Goal Vel: %5.7f\r\n",goal_vel);
    printf("Cur Vel: %5.7f\r\n",cur_vel);
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

// Function helpers for generating parameters
static inline float generate_b0(controller_t * K){ // must have acquired mutex,maybe we should change it to recursive mutex
    float Ts = VEL_SAMPLE_TIME; // for easier reading
    float N = K->N;  // for easier reading
    return K->P*(1+N*Ts) + 
            K->I*Ts*(1 + N*Ts) + 
            K->D*N; // Check that this is true
            // https://www.scilab.org/discrete-time-pid-controller-implementation
}

static inline float generate_b1(controller_t * K){// must have acquired mutex,maybe we should change it to recursive mutex
    float Ts = VEL_SAMPLE_TIME; // for easier reading
    float N = K->N;  // for easier reading
    return -1.0*(K->P*(2+N*Ts) + K->I*Ts + 2*K->D*N);
}

static inline float generate_b2(controller_t * K){// must have acquired mutex,maybe we should change it to recursive mutex
    float Ts = VEL_SAMPLE_TIME; // for easier reading
    float N = K->N;  // for easier reading
    return K->P + K->D * N;
}

static inline float generate_a0(controller_t * K){// must have acquired mutex,maybe we should change it to recursive mutex
    float Ts = VEL_SAMPLE_TIME; // for easier reading
    float N = K->N;  // for easier reading
    return (1+N*Ts);
}
static inline float generate_a1(controller_t * K){// must have acquired mutex,maybe we should change it to recursive mutex
    float Ts = VEL_SAMPLE_TIME; // for easier reading
    float N = K->N;  // for easier reading
    return -1.0*(2+N*Ts);
}
static inline float generate_a2(controller_t * K){// must have acquired mutex,maybe we should change it to recursive mutex
    float Ts = VEL_SAMPLE_TIME; // for easier reading
    float N = K->N;  // for easier reading
    return 1.0;
}


float generate_set_duty(side_t duty_side) // This is the controller
{
    if (duty_side == RIGHT){
        printf("Right Side\r\n");
    }
    else{
        printf("Left Side\r\n");
    }
    controller_t * params = NULL;
    mutex_t * controller_side_mutex = NULL;
    if (duty_side == LEFT){
        params = &controller_params_L;
        controller_side_mutex = &controller_params_L_mtx;
    }
    else if (duty_side == RIGHT){
        params = &controller_params_R;
        controller_side_mutex = &controller_params_R_mtx;
    }
    else{
        // printf("WARNING:No side for generate set duty function");
    }


    float error = get_error(duty_side);
    static float prev_1_cmd = 0.0; // need a way for this to get set to fix amount before startup?
    static float prev_2_cmd = 0.0;
    static float prev_1_error = 0.0;
    static float prev_2_error = 0.0;

    float b [3];
    float a [3];
    mutex_enter_blocking(controller_side_mutex);
    b[0] = generate_b0(params);
    b[1] = generate_b1(params);
    b[2] = generate_b2(params);
    a[0] = generate_a0(params);
    a[1] = generate_a1(params);
    a[2] = generate_a2(params);
    mutex_exit(controller_side_mutex);

    // Get a raw fix value first
    float cmd = -1*a[1]*prev_1_cmd/a[0] - 
                        a[2]*prev_2_cmd/a[0] + 
                        b[0]*error/a[0] +
                        b[1]*prev_1_error/a[0] +
                        b[2]*prev_2_error/a[0]; // Difference equation

    prev_2_cmd = prev_1_cmd;
    prev_1_cmd = cmd;
    prev_2_error = prev_1_error;
    prev_1_error = error;

    // Then need to saturate this value from -1 to 1
    float cmd_sat = saturate(cmd);

    // Then need to map to duty cycle which is from 0.1 - 0.2
    float duty = map(cmd_sat);


    // printf("Error: %5.7f\r\n", error);
    // printf("Cmd: %5.7f\r\n", cmd);
    // printf("Cmd Sat: %5.7f\r\n", cmd_sat);
    // printf("Duty: %5.7f\r\n", duty);

    

    // duty = 0.150005;

    // Test to find vel to duty mapping
    duty = get_goal_velocity(duty_side) * 3.8372 + 0.0075;
    duty = saturate(duty);
    duty = map(duty);
    
    printf("Duty: %5.7f\r\n", duty);


    return duty;
}
