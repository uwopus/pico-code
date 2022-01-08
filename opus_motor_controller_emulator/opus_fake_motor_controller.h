/**
 * Copyright (c) 2022 Opus
 */

#ifndef OPUSFAKEMOTORCONTROLLER_H
#define OPUSFAKEMOTORCONTROLLER_H

#include "stdio.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#define pENCODER_A 0 // we want these both to be on the same slice to share the same clock
#define pENCODER_B 1
#define MAX_FAKE_VEL 100000

void init_fake_encoder_output();


#endif

