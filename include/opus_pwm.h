/**
 * Copyright (c) 2022 Opus
 */

#ifndef OPUS_PWM_H
#define OPUS_PWM_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "stdio.h"
#include "opus_shared_definitions.h"
#include "stdint.h"


#define pPWM_LEFT_GPIO 28
#define pPWM_RIGHT_GPIO 16
#define PWM_WRAP UINT16_MAX // Max is 65535, increase to have more precision in duty cycles, decrease to have quicker response time to channel change
#define PWM_FREQ 20 // Hz. Based off range of motor controller

uint set_pwm(side_t set_pwm_side, float duty_cycle);

uint init_pwm(side_t init_pwm_side,uint cycles);

#endif