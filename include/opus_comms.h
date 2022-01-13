#ifndef OPUS_COMMS_H
#define OPUS_COMMS_H

#include "stdint.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"

#define MAX_PKT_SIZE UINT8_MAX

typedef enum packet_types {
    INIT = 0, 
    HEARTBEAT = 1,
    STATE = 2, 
    PWM = 3, 
    ENC = 4
} opus_packet_type_t;

typedef struct packet {
    uint32_t t_ms; // sequence number, monotonically increasing until wraparound 
    opus_packet_type_t type;
    uint8_t len;
    uint8_t data[MAX_PKT_SIZE];
} opus_packet_t;

void comms_init(bool is_slave);
void send_data(const uint8_t *src, size_t len);
void receive_data(uint8_t *dst, size_t len);

void parse_packet(opus_packet_t packet);
void send_packet(opus_packet_type_t type, void *data, uint8_t len);
void update();
#endif