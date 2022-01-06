#include "opus_pwm.h"
#include "hardware/pwm.h"

int main() {

    init_pwm(pPWM_LEFT_GPIO, PWM_WRAP);
    init_pwm(pPWM_RIGHT_GPIO, PWM_WRAP);

    // Sample on how to change the PWM
    set_pwm(pPWM_LEFT_GPIO,0.5);
    set_pwm(pPWM_RIGHT_GPIO,0.5);

    float dc = 0;

    while(1) {
        dc += 0.01;
        set_pwm(pPWM_LEFT_GPIO, dc);
        busy_wait_ms(10);

        if(dc > 1){
            dc = 0;
        }
    }
    
}