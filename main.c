#include "opus_pwm.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"
#include "opus_comms.h"
#include "opus_encoder.h"
#include "opus_shared_definitions.h"
#include "pico/multicore.h"

void core1_main();
void init_opus();

int main() {

    // init_pwm(pPWM_LEFT_GPIO, PWM_WRAP);
    // init_pwm(pPWM_RIGHT_GPIO, PWM_WRAP);

    // // Sample on how to change the PWM
    // set_pwm(pPWM_LEFT_GPIO,0.5);
    // set_pwm(pPWM_RIGHT_GPIO,0.5);


    init_opus();
    multicore_launch_core1(core1_main); // Control Loop Core. 

    while(1) {

    }
    
}

void init_opus(){
    stdio_init_all(); //Not sure if this should remain here or go else where
    init_encoders();
    printf("Opus Started");

}

void core1_main(){

    int32_t count_left = 0;
    int32_t count_right = 0;

    int32_t prev_count_left = 0;
    int32_t prev_count_right = 0;

    bool been_updated = true; // Start as true to print the count on init

    while(1) {

        count_left = get_encoder_count(LEFT);
        count_right = get_encoder_count(RIGHT);

        if(prev_count_left != count_left){
            prev_count_left = count_left;
            been_updated = true;
        }
        else if (prev_count_right != count_right){
            prev_count_right = count_right;
            been_updated = true;
        }

        if (been_updated)
        {
            printf("Core 1: Left count: %8d | Right count: %8d\n", count_left,count_right);
            been_updated = false;
        }
    }
}