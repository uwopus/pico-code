#ifndef OPUS_COMMS_H
#define OPUS_COMMS_H

#include "stdint.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"

#define MAX_PKT_SIZE 10


void comms_init(bool is_slave);
void send_data(const uint8_t *src, size_t len);

#endif