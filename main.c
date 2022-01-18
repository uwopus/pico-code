#include "opus_pwm.h"
#include "opus_comms.h"
#include "opus_encoder.h"
#include "opus_shared_definitions.h"
#include "pico/multicore.h"
#include "opus_velocity.h"

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
        if(sem_acquire_timeout_ms(&sem_spi_rx, 1)){
            parse_packet();
        }

        sleep_ms(1);

    }
    
}

void init_opus(){
    stdio_init_all(); //Not sure if this should remain here or go else where
    init_encoders();
    comms_init(true);
    printf("Opus Started");
    init_velocity();
}

void core1_main(){ // velocity controller
    float duty_L = 0.5; // I know don't need two but for readability maybe?
    float duty_R = 0.5;
    while (true)
    {
        duty_L = generate_set_duty(LEFT);
        set_pwm(LEFT,duty_L);
        duty_R = generate_set_duty(RIGHT);
        set_pwm(RIGHT,duty_R);
    }
}