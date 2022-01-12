#include "opus_pwm.h"
#include "hardware/pwm.h"
#include "opus_encoder.h"
#include "opus_shared_definitions.h"

int main() {

    init_pwm(pPWM_LEFT_GPIO, PWM_WRAP);
    init_pwm(pPWM_RIGHT_GPIO, PWM_WRAP);

    float pwm_val = 0.5;
    while(true) {
        set_pwm(pPWM_LEFT_GPIO,pwm_val);
        set_pwm(pPWM_RIGHT_GPIO,pwm_val);
        busy_wait_ms(1);
    }

    // printf("Program Started");


    // init_encoders();


    // int32_t count_left = 0;
    // int32_t count_right = 0;

    // int32_t prev_count_left = 0;
    // int32_t prev_count_right = 0;

    // bool been_updated = true; // Start as true to print the count on init

    // while(1) {

    //     count_left = get_encoder_count(LEFT);
    //     count_right = get_encoder_count(RIGHT);

    //     if(prev_count_left != count_left){
    //         prev_count_left = count_left;
    //         been_updated = true;
    //     }
    //     else if (prev_count_right != count_right){
    //         prev_count_right = count_right;
    //         been_updated = true;
    //     }

    //     if (been_updated)
    //     {
    //         printf("Left count: %8d | Right count: %8d\n", count_left,count_right);
    //         been_updated = false;
    //     }
    // }
    
}