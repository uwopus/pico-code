#ifndef OPUS_COMMS_H
#define OPUS_COMMS_H

#include "stdint.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "pico/sync.h"

typedef struct opus_pico_tx_packet {
    uint32_t t_ms; // sequence number, monotonically increasing until wraparound 
    int32_t L_encd_ticks;
    float L_cur_vel;
    float L_goal_vel;
    int32_t R_encd_ticks;
    float R_cur_vel;
    float R_goal_vel;
    struct {
        uint8_t state;
        uint8_t pad1;
        uint8_t pad2;
        uint8_t crc;
    } state_pad_pad_crc;
} opus_pico_tx_packet_t;

typedef struct opus_pico_rx_packet {
    uint32_t t_ms; // sequence number, monotonically increasing until wraparound 
    float L_vel_cmd;
    float R_vel_cmd;
    uint8_t state_cmd;
    uint8_t state_pad[3];
    uint8_t reserved[15];
    uint8_t crc;
} opus_pico_rx_packet_t;

void comms_init();
void handle_packets();
uint8_t crc8(const void* ptr, int len);

#endif