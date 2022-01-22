#ifndef OPUS_COMMS_H
#define OPUS_COMMS_H

#include "stdint.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "pico/sync.h"

#define MAX_PKT_SIZE 10

typedef enum packet_types {
    PKT_TYPE_INIT = 0, 
    PKT_TYPE_ACK = 1,
    PKT_TYPE_HEARTBEAT = 2,
    PKT_TYPE_STATE = 3, 
    PKT_TYPE_VEL = 4,
    PKT_TYPE_ENC = 5
} opus_packet_type_t;

typedef struct packet {
    uint32_t t_ms; // sequence number, monotonically increasing until wraparound 
    opus_packet_type_t type;
    uint8_t len;
    uint8_t data[MAX_PKT_SIZE];
    // uint8_t crc;
} opus_packet_t;


void comms_init(bool is_slave);

void parse_packet();
void send_packet(opus_packet_type_t type, void *data, uint8_t len);
void update();
#endif