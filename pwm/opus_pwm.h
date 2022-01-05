/**
 * Copyright (c) 2022 Opus
 */



#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define pPWM_LEFT_GPIO 28
#define pPWM_RIGHT_GPIO 16


uint set_pwm(uint pin, float duty_cycle);

uint init_pwm(uint GPIO_Pin,uint cycles);

