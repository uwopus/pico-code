#include "opus_comms.h"
#include "string.h"
#include "hardware/irq.h"
#include "opus_shared_definitions.h"

#define OPUS_SPI_PORT spi0
#define OPUS_BAUDRATE 500E3

#define pOPUS_SPI_SCK 2
#define pOPUS_SPI_MISO 3
#define pOPUS_SPI_MOSI 4
#define pOPUS_SPI_CS 5

#define pOPUS_SPI_ACTIVITY 8 // on while RX or TX on SPI. We should have put little LEDs on MOSI/MISO...

#define OPUS_SPI_PINS ((1 << pOPUS_SPI_SCK) | (1 << pOPUS_SPI_MISO) | (1 << pOPUS_SPI_MOSI) | (1 << pOPUS_SPI_CS))

void packet_available(){ 
    sem_release(&comm_pkt_available_sem);
    gpio_xor_mask(1 << 8);
    // uint16_t data = spi0_hw->dr;
    hw_clear_bits(spi0_hw->imsc, SPI_SSPIMSC_RXIM_BITS);
    irq_clear(SPI0_IRQ);

}

void comms_init(bool is_slave) {

    gpio_init(8);
    gpio_set_dir(8, GPIO_OUT);

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

    // set up IRQ for incoming data.
    sem_init(&comm_pkt_available_sem, 0, 1); 
    irq_set_enabled(SPI0_IRQ, true);
    irq_set_exclusive_handler(SPI0_IRQ, packet_available);

    // Enable SPI0 RX IRQ
    spi0_hw->imsc |= SPI_SSPIMSC_RXIM_BITS;
    // spi0_hw->cr0 |= 0b1111 & SPI_SSPCR0_DSS_BITS;

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

void send_packet(opus_packet_type_t type, void *data, uint8_t len){ 
    opus_packet_t packet; 
    absolute_time_t absTime = get_absolute_time();
    packet.t_ms = to_ms_since_boot(absTime);
    packet.type = type;
    packet.len = len;
    memcpy(packet.data, data, len);

    void* pktPtr = (void*) &packet;
    send_data(pktPtr, sizeof(opus_packet_t));

}