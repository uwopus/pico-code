#include "opus_comms.h"

#define OPUS_SPI_PORT spi0
#define OPUS_BAUDRATE 10e6

#define OPUS_SPI_SCK 2
#define OPUS_SPI_MISO 3
#define OPUS_SPI_MOSI 4
#define OPUS_SPI_CS 5

#define OPUS_SPI_PINS ((1 << OPUS_SPI_SCK) | (1 << OPUS_SPI_MISO) | (1 << OPUS_SPI_MOSI) | (1 << OPUS_SPI_CS))

void comms_init(bool is_slave) {
    spi_init(OPUS_SPI_PORT, OPUS_BAUDRATE);
    gpio_init_mask(OPUS_SPI_PINS);
    gpio_set_function(OPUS_SPI_SCK, GPIO_FUNC_SPI);
    gpio_set_function(OPUS_SPI_MISO, GPIO_FUNC_SPI);
    gpio_set_function(OPUS_SPI_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(OPUS_SPI_CS, GPIO_FUNC_SPI);

    if(is_slave) {
        spi_set_slave(OPUS_SPI_PORT, true); 
        gpio_set_dir(OPUS_SPI_MISO, GPIO_OUT);
    } else {
        gpio_set_dir(OPUS_SPI_SCK, GPIO_OUT);
        gpio_set_dir(OPUS_SPI_MOSI, GPIO_OUT);
        gpio_set_dir(OPUS_SPI_CS, GPIO_OUT);
    }
}

void send_data(const uint8_t *src, size_t len){
    spi_write_blocking(OPUS_SPI_PORT, src, len);
}