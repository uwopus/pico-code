/**
 * Copyright (c) 2022 Opus
 * LED Library for Various LED Functions. 
 */
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "pico/sync.h"
#include "opus_led.h"

opus_led_t leds[OPUS_NUM_LEDS];
mutex_t led_mtxs[OPUS_NUM_LEDS];

const uint8_t pOPUS_LEDS[OPUS_NUM_LEDS] = {pOPUS_LED_1, pOPUS_LED_2, pOPUS_INBUILT_LED};
const uint32_t OPUS_LED_MASK = (1 << pOPUS_LED_1) | (1 << pOPUS_LED_2) | (1 << pOPUS_INBUILT_LED);

void led_init(){
    gpio_init_mask(OPUS_LED_MASK);
    gpio_set_dir_out_masked(OPUS_LED_MASK); 

    for(int i = 0; i < OPUS_NUM_LEDS; i++) {
        mutex_init(&led_mtxs[i]);

        leds[i].mode = LED_MODE_OFF;
        leds[i].pin = pOPUS_LEDS[i];
        leds[i].curState = 0;

        for(int j = 0; j < OPUS_LED_NUM_PARAMS; j++) {
            leds[i].params[j] = 0;
        }

        leds[i].last_time_toggled = get_absolute_time();
    }
}

void opus_led_change_struct(uint8_t led_num, opus_led_mode_t mode, uint16_t* params, uint8_t params_len){
    if(led_num >= OPUS_NUM_LEDS || led_num < 0) {
        return;
    }

    opus_led_t* led = &leds[led_num];

    mutex_enter_blocking(&led_mtxs[led_num]);

    led->mode = mode;
    for(int i = 0; i < params_len; i++) {
        led->params[i] = params[i];
    }

    mutex_exit(&led_mtxs[led_num]);
}

void put_led(uint8_t led_num, bool value){
    uint16_t val = (uint16_t) value;
    opus_led_change_struct(led_num, LED_MODE_STEADY, &val, 1);
}

void blink_led(uint8_t led_num, uint16_t time_on_ms, uint16_t time_off_ms){
    uint16_t val[2] = {};
    val[0] = time_on_ms;
    val[1] = time_off_ms;

    opus_led_change_struct(led_num, LED_MODE_BLINKING, val, 2);
}

bool opus_led_update(){
    for(int i = 0; i < OPUS_NUM_LEDS; i++) {
        if(mutex_enter_timeout_ms(&led_mtxs[i], 1)) {
            opus_led_t* led = &leds[i];

            if(led->mode == LED_MODE_OFF) {
                if (led->curState != 0) {
                    gpio_put(led->pin, 0);
                    led->curState = 0;
                }
            } else if (led->mode == LED_MODE_STEADY) {
                if (led->curState != led->params[0]){
                    gpio_put(led->pin, led->params[0]);
                    led->curState = led->params[0];
                }
            } else if (led->mode == LED_MODE_BLINKING) {
                absolute_time_t curTime = get_absolute_time();
                uint16_t deltaTime = led->curState ? led->params[0] : led->params[1];
                
                int64_t timeDiff = absolute_time_diff_us(led->last_time_toggled, curTime)/1000;
                if(timeDiff > UINT16_MAX) {
                    timeDiff = UINT16_MAX;
                }

                uint16_t timeDiff_16 = (uint16_t) timeDiff;

                if (timeDiff_16 >= deltaTime) {
                    gpio_put(led->pin, !led->curState);
                    led->curState = !led->curState;
                    led->last_time_toggled = curTime;
                }
            }

            mutex_exit(&led_mtxs[i]);
        }
    }

    return true; // for repeating timer! not used right now tho...
}