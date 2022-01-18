#include "opus_pwm.h"


uint init_pwm(side_t init_pwm_side,uint cycles){
    uint GPIO_Pin = pPWM_LEFT_GPIO; // I know this redundant
    if (init_pwm_side == LEFT)
    {
        GPIO_Pin = pPWM_LEFT_GPIO;
    }
    else if (init_pwm_side == RIGHT)
    {
        GPIO_Pin = pPWM_RIGHT_GPIO;
    }
    else
    {
        // printf("WARNING: No side for init_pwm");
    }
    // Tell GPIO # They are allocated to the PWM
    gpio_set_function(GPIO_Pin, GPIO_FUNC_PWM);
    // Find out which PWM slice is connected to GPIO #
    uint slice_num = pwm_gpio_to_slice_num(GPIO_Pin);

    // Set period of # cycles (0 to cycles inclusive)
    pwm_set_wrap(slice_num, cycles);

    // Set channel # output high for cycles/2 cycles before dropping
    set_pwm(GPIO_Pin, 0.5);

    // Set the PWM running
    pwm_set_enabled(slice_num, true);
    return slice_num;
}

uint set_pwm(side_t set_pwm_side, float duty_cycle) {
    uint GPIO_Pin = pPWM_LEFT_GPIO; // I know this redundant
    if (set_pwm_side == LEFT)
    {
        GPIO_Pin = pPWM_LEFT_GPIO;
    }
    else if (set_pwm_side == RIGHT)
    {
        GPIO_Pin = pPWM_RIGHT_GPIO;
    }
    else
    {
        // printf("WARNING: No side for set_pwm");
    }
    uint status = 0;

    if (duty_cycle > 1) {
        duty_cycle = 1;
        // printf("set_pwm: WARNING: duty_cycle > 1");
    } else if (duty_cycle < 0) {
        duty_cycle = 0;
        // printf("set_pwm: WARNING: duty_cycle < 0");
    }
    
    int count = PWM_WRAP*duty_cycle;
        
    pwm_set_gpio_level(GPIO_Pin, count);
    return status;
}
