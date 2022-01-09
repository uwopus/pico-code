/**
 * Copyright (c) 2022 Opus
 */

#ifndef OPUS_PIO_ENCODER_H
#define OPUS_PIO_ENCODER_H
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/irq.h"


#include "opus_pio_encoder.pio.h"
void rotary_encoder_init(uint rotary_encoder_A,uint pio_num);

static void pio0_irq_handler();
static void pio1_irq_handler();



#endif