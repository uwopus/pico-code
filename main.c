#include "opus_pwm.h"
#include "hardware/pwm.h"
#include "opus_fake_motor_controller.h"

#define pADC 26 // Can only do 26,27,28,29

int main() {
    stdio_init_all();

    adc_init();
    
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(pADC);
    // Select ADC input 0 (GPIO26)
    adc_select_input(pADC-26);

    const float conversion_factor = MAX_FAKE_VEL / (1 << 12);
    uint16_t result = adc_read();

    init_fake_encoder_output();

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
    while(1) {
        // Read in the desired motor velocity from adc
        uint16_t result = adc_read();
        // Convert adc readings to this arbitrary velocity
        uint motor_vel = conversion_factor * result;

        // Keep constant velocity for now
        motor_vel = 1;

        uint leadEncoder = 0; // 0 off , maybe make this an enum
        uint lagEncoder = 0;

        bool reverse = false; // not sure how reverse is set so for now its like this

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
            printf("Rotations: %d\n",num_of_rotations);
        }

    }
    
}