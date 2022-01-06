/**
 * Copyright (c) 2022 Opus
 * PWM Library for Motor Controller
 */
#include "opus_fake_motor_controller.h"


void init_fake_encoder_output()
{

    gpio_init(pENCODER_A);
    gpio_set_dir(pENCODER_A, GPIO_OUT);
    gpio_init(pENCODER_B);
    gpio_set_dir(pENCODER_B, GPIO_OUT);

    // Initialise as 0
    gpio_put(pENCODER_A, 0);
    gpio_put(pENCODER_B, 0);
    
}
