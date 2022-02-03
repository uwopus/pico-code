/**
 * Copyright (c) 2022 Opus
 */

#ifndef OPUS_LED_H
#define OPUS_LED_H

#include "stdint.h"
#include "pico/mutex.h"

#define OPUS_NUM_LEDS 3
#define OPUS_LED_NUM_PARAMS 3

#define pOPUS_LED_1 8
#define pOPUS_LED_2 17
#define pOPUS_INBUILT_LED 25


typedef enum { 
    LED_MODE_OFF,
    LED_MODE_STEADY,
    LED_MODE_BLINKING
} opus_led_mode_t;

typedef struct {
    opus_led_mode_t mode; 
    uint8_t pin; 
    uint16_t params[OPUS_LED_NUM_PARAMS]; // What these mean depend on mode.
    bool curState;
    absolute_time_t last_time_toggled; 
} opus_led_t;

void led_init();
void put_led(uint8_t led_num, bool value); 
void blink_led(uint8_t led_num, uint16_t time_on_ms, uint16_t time_off_ms); // PWM blinking is unsupported on LED2 because of conflict with driving PWM. 
bool opus_led_update();

#endif // OPUS_LED_H