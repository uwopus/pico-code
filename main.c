#include "opus_pwm.h"


#define DEBUG 0



int main() {
    init_pwm(pPWM_LEFT_GPIO, PWM_WRAP);
    init_pwm(pPWM_RIGHT_GPIO, PWM_WRAP);

    // Sample on how to change the PWM
    set_pwm(pPWM_LEFT_GPIO,0.5);
    set_pwm(pPWM_RIGHT_GPIO,0.5);
    
}