/**
 * Copyright (c) 2022 Opus
 * PWM Library for Motor Controller
 */

#define PWM_WRAP 10000 // Max is 65536, increase to have more precision in duty cycles, decrease to have quicker response time to channel change

uint init_pwm(uint GPIO_Pin,uint cycles){
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

uint set_pwm(uint pin, float duty_cycle) {
    uint status = 0;

    if (duty_cycle > 1) {
        duty_cycle = 1;
        if(DEBUG) {
            printf("set_pwm: WARNING: duty_cycle > 1");
        }
    } else if (duty_cycle < 0) {
        duty_cycle = 0;
        if(DEBUG) {
            printf("set_pwm: WARNING: duty_cycle < 0");
        }
    }
    
    int count = PWM_WRAP*duty_cycle;
        
    pwm_set_gpio_level(pPWM_LEFT_GPIO, count);
    return status;
}
