#ifndef OPUS_COMMS_H
#define OPUS_COMMS_H

#include "stdint.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "pico/sync.h"

#define COMMS_DATA_ARR_LEN (13) // the total will be byte aligned to word-sizes.

typedef enum packet_types {
    PKT_TYPE_INIT = 0, 
    PKT_TYPE_ACK = 1,
    PKT_TYPE_HEARTBEAT = 2,
    PKT_TYPE_STATE = 3, 
    PKT_TYPE_SET_VEL = 4,
    PKT_TYPE_GET_VEL = 5,
    PKT_TYPE_SET_CONFIG = 6,  
    PKT_TYPE_ENC = 7
} opus_packet_type_t;

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

// typedef struct opus_pico_tx_packet {
//     uint32_t t_ms; // sequence number, monotonically increasing until wraparound 
//     int32_t L_encd_ticks;
//     uint32_t L_cur_vel;
//     uint32_t L_goal_vel;
//     int32_t R_encd_ticks;
//     uint32_t R_cur_vel;
//     uint32_t R_goal_vel;
//     // uint8_t state_pad_pad_crc[4];
// } opus_pico_tx_packet_t;


typedef struct opus_pico_rx_packet {
    uint32_t t_ms; // sequence number, monotonically increasing until wraparound 
    float L_vel_cmd;
    float R_vel_cmd;
    uint8_t state_cmd;
    uint8_t state_pad[3];
    uint8_t reserved[15];
    uint8_t crc;
} opus_pico_rx_packet_t;



void comms_init(bool is_slave);
uint8_t crc8(const void* ptr, int len);

void receive_packet();
void handle_packets();
void send_packet(opus_packet_type_t type, void *data, uint8_t len);
void update();
#endif