/**
 * Copyright (c) 2022 Opus
 * LED Library for Various LED Functions. 
 */
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "opus_led.h"

void init_leds(){
    gpio_init_mask(OPUS_LED_MASK);
    gpio_set_dir_out_masked(OPUS_LED_MASK);
    

    

}

void put_led(uint8_t led_num, bool value){

}

void blink_led(uint8_t led_num, uint16_t time_on_ms, uint16_t time_off_ms){

}