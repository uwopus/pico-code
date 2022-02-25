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

typedef struct packet {
    uint32_t t_ms; // sequence number, monotonically increasing until wraparound 
    opus_packet_type_t type;
    uint8_t len;
    // uint8_t RESERVED[3]; // for byte alignment.
    uint8_t data[COMMS_DATA_ARR_LEN];
    // uint8_t crc;
} opus_packet_t;


void comms_init(bool is_slave);

void recieve_packet();
void parse_packet();
void send_packet(opus_packet_type_t type, void *data, uint8_t len);
void update();
#endif