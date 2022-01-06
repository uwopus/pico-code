/**
 * Copyright (c) 2022 Opus
 */



#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define pPWM_LEFT_GPIO 0
#define pPWM_RIGHT_GPIO 1
#define PWM_WRAP 10000 // Max is 65536, increase to have more precision in duty cycles, decrease to have quicker response time to channel change


uint set_pwm(uint pin, float duty_cycle);

uint init_pwm(uint GPIO_Pin,uint cycles);

