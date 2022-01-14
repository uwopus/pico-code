/**
 * Copyright (c) 2022 Opus
 */

#ifndef OPUS_LED_H
#define OPUS_LED_H

#include "stdint.h"
#include "pico/mutex.h"

#define pOPUS_LED_1 8
#define pOPUS_LED_2 17

const uint8_t pOPUS_LEDS[2] = {pOPUS_LED_1, pOPUS_LED_2};

const uint32_t OPUS_LED_MASK = (1 << pOPUS_LED_1) | (1 << pOPUS_LED_2);

void init_leds();
void put_led(uint8_t led_num, bool value); 
void blink_led_1(uint16_t time_on_ms, uint16_t time_off_ms); // PWM blinking is unsupported on LED2 because of conflict with driving PWM. 


#endif // OPUS_LED_H