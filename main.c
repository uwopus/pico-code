#include "opus_pwm.h"
#include "opus_comms.h"
#include "opus_encoder.h"
#include "opus_shared_definitions.h"
#include "pico/multicore.h"
#include "opus_velocity.h"

void core1_main();
void init_opus_all();
void init_opus_core0();
void init_opus_core1();

int main() {

    init_opus_all();
    multicore_launch_core1(core1_main); // Control Loop Core. 

    gpio_init(8);
    gpio_set_dir(8, GPIO_OUT);

    init_opus_core0();

    while(1) {
        handle_packets();
    }
    
}

void init_opus_all(){
    stdio_init_all();
}

void init_opus_core0(){
    comms_init();
    printf("Opus Started");
}

void init_opus_core1(){
    printf("Opus Started Core 1");
    init_pwm(LEFT,PWM_WRAP);
    init_pwm(RIGHT,PWM_WRAP);
    init_encoders();
    init_velocity();
}

void core1_main(){ // velocity controller

    init_opus_core1();


    float duty_L = 0.15; // I know don't need two but for readability maybe?
    float duty_R = 0.15;

    //mutex_enter_blocking(&VEL_GOAL_L_MTX);
    vel_goal_L = 0.0;
    //mutex_exit(&VEL_GOAL_L_MTX);

    gpio_put(8,1);

    while (true)
    {
        // non_timer_update_velocity_pwm();
        sleep_ms(1);
    }
}