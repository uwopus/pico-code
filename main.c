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
        gpio_put(8, 1);
        recieve_packet();
        parse_packet();
    }
    
}

void init_opus_all(){
    stdio_init_all();
}

void init_opus_core0(){
    comms_init(true);
    printf("Opus Started");
}

void init_opus_core1(){
    printf("Opus Started Core 1");
    init_pwm(LEFT,PWM_WRAP);
    init_pwm(RIGHT,PWM_WRAP);
    init_encoders();
    init_velocity();

    // set_pwm(LEFT,0.5); // Init at 0.5 which is stop
    // set_pwm(RIGHT,0.5); // Init at 0.5 which is stop

}

void core1_main(){ // velocity controller

    init_opus_core1();


    float duty_L = 0.15; // I know don't need two but for readability maybe?
    float duty_R = 0.15;
    float change = 0.000;// 0.0001;


    mutex_enter_blocking(&VEL_GOAL_L_MTX);
    vel_goal_L = 0.0;
    mutex_exit(&VEL_GOAL_L_MTX);

    while (true)
    {
        // if (duty_L > 0.2 || duty_L < 0.1)
        // {
        //     change *= -1;
        // }
        // duty_L += change;
        // duty_R += 0.01
        // duty_L = generate_set_duty(LEFT); // this now handled by the timer
        // set_pwm(LEFT,duty_L);
        // duty_R = generate_set_duty(RIGHT);
        // set_pwm(RIGHT,duty_R);

        // int32_t ticks = get_encoder_count(LEFT).ticks;

        // printf("Duty: %8.6f, Current Ticks: %d\n\r",duty_L, ticks);
        sleep_ms(200);
    }
}