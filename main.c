#include "opus_pwm.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "opus_fake_motor_controller.h"

#define pMEASURE 5

float measure_duty_cycle(uint gpio) {
    // Only the PWM B pins can be used as inputs.
    assert(pwm_gpio_to_channel(gpio) == PWM_CHAN_B);
    uint slice_num = pwm_gpio_to_slice_num(gpio);

    // Count once for every 100 cycles the PWM B input is high
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv_mode(&cfg, PWM_DIV_B_HIGH);
    pwm_config_set_clkdiv(&cfg, 100);
    pwm_init(slice_num, &cfg, false);
    gpio_set_function(gpio, GPIO_FUNC_PWM);

    pwm_set_enabled(slice_num, true);
    sleep_ms(10);
    pwm_set_enabled(slice_num, false);
    float counting_rate = clock_get_hz(clk_sys) / 100;
    float max_possible_count = counting_rate * 0.01;
    return pwm_get_counter(slice_num) / max_possible_count;
}


int main() {
    stdio_init_all();
    printf("\nPWM duty cycle measurement example\n");

    uint32_t count = 0;


    /* How this works is each encoder channel is split into 4 sections
        2 of these sections the signal is high, the other 2 low
        The other channel is 1 section offset.
        Which section is being broadcasted is dependent on count which acts as our time incrementer
        At Maximum velocity, we want each of the sections being plaid onces through,
        To establish this, the sections are played at MAX_FAKE_VEL increments apart
        , and count can be incremented at a max of MAX_FAKE_VEL
        Count then loops after 4.
        But count will increment at a rate that is the velocity.

        */

       uint num_of_rotations = 0;
       printf("Rotations: %d\n",num_of_rotations);
        bool reverse = false;

    while(1) {
        // Read in the desired motor velocity from adc
        float duty_cycle = measure_duty_cycle(pMEASURE);
        // Convert adc readings to this arbitrary velocity
        uint motor_vel = 2 * MAX_FAKE_VEL * duty_cycle;


        if (motor_vel - MAX_FAKE_VEL >= 0) //meaning requested a positive velocity
        {
            motor_vel -= MAX_FAKE_VEL; // Need to fix around the 50% amount
            reverse = false;
        }
        else//meaning negative velocity
        {
            motor_vel = -1*(motor_vel - MAX_FAKE_VEL); // this will still give motor_vel as positive since the condition to get here makes sure of it
            reverse = true;
        }


        uint leadEncoder = 0; // 0 off , maybe make this an enum
        uint lagEncoder = 0;


        if (count < MAX_FAKE_VEL)
        {
            leadEncoder = 1;
            lagEncoder = 0;
        }
        else if (count < MAX_FAKE_VEL * 2)
        {
            leadEncoder = 1;
            lagEncoder = 1;
        }
        else if (count < MAX_FAKE_VEL * 3)
        {
            leadEncoder = 0;
            lagEncoder = 1;
        }
        else
        {
            leadEncoder = 0;
            lagEncoder = 0;
        }
        
        if (reverse){
            gpio_put(pENCODER_A,lagEncoder);
            gpio_put(pENCODER_B,leadEncoder);
        }
        else{
            gpio_put(pENCODER_A,leadEncoder);
            gpio_put(pENCODER_B,lagEncoder);
        }

        count += motor_vel;

        if (count > 4*MAX_FAKE_VEL){ // Want the count to go up by 
            count = 0;
            num_of_rotations++;
        }
        
        printf("Duty Reading: %.1f%% \n",duty_cycle*100.f);
        printf("Rotations: %d\n",num_of_rotations);

    }
    
}