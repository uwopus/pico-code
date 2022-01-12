#include "opus_comms.h"

#define OPUS_SPI_PORT spi0
#define OPUS_BAUDRATE 500E3

#define pOPUS_SPI_SCK 2
#define pOPUS_SPI_MISO 3
#define pOPUS_SPI_MOSI 4
#define pOPUS_SPI_CS 5

#define pOPUS_SPI_ACTIVITY 8 // on while RX or TX on SPI. We should have put little LEDs on MOSI/MISO...

#define OPUS_SPI_PINS ((1 << pOPUS_SPI_SCK) | (1 << pOPUS_SPI_MISO) | (1 << pOPUS_SPI_MOSI) | (1 << pOPUS_SPI_CS))

void comms_init(bool is_slave) {
    spi_init(OPUS_SPI_PORT, OPUS_BAUDRATE);
    gpio_init_mask(OPUS_SPI_PINS | (1 << pOPUS_SPI_ACTIVITY));
    gpio_set_function(pOPUS_SPI_SCK, GPIO_FUNC_SPI);
    gpio_set_function(pOPUS_SPI_MISO, GPIO_FUNC_SPI);
    gpio_set_function(pOPUS_SPI_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(pOPUS_SPI_CS, GPIO_FUNC_SPI);

    gpio_set_dir(pOPUS_SPI_ACTIVITY, GPIO_OUT);

    if(is_slave) {
        spi_set_slave(OPUS_SPI_PORT, true); 
        gpio_set_dir(pOPUS_SPI_MISO, GPIO_OUT);
    } else {
        gpio_set_dir(pOPUS_SPI_SCK, GPIO_OUT);
        gpio_set_dir(pOPUS_SPI_MOSI, GPIO_OUT);
        gpio_set_dir(pOPUS_SPI_CS, GPIO_OUT);
    }
}

void send_data(const uint8_t *src, size_t len){
    gpio_put(pOPUS_SPI_ACTIVITY, true);
    spi_write_blocking(OPUS_SPI_PORT, src, len);
    gpio_put(pOPUS_SPI_ACTIVITY, false);
}

void receive_data(uint8_t *dst, size_t len){ 
    gpio_put(pOPUS_SPI_ACTIVITY, true);
    spi_read_blocking(OPUS_SPI_PORT, 0x00, dst, len);
    gpio_put(pOPUS_SPI_ACTIVITY, false);
}