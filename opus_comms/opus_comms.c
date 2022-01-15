#include "opus_comms.h"
#include "string.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "opus_shared_definitions.h"

#define OPUS_SPI_PORT spi0
#define OPUS_BAUDRATE 500E3

#define pOPUS_SPI_SCK 2
#define pOPUS_SPI_MISO 3
#define pOPUS_SPI_MOSI 4
#define pOPUS_SPI_CS 5

#define OPUS_SPI_PINS ((1 << pOPUS_SPI_SCK) | (1 << pOPUS_SPI_MISO) | (1 << pOPUS_SPI_MOSI) | (1 << pOPUS_SPI_CS))

void dma_irq() {
    sem_release(&sem_spi_rx);
    dma_irqn_acknowledge_channel(0, spi_dma_rx);
    irq_clear(DMA_IRQ_0);
}
void comms_init(bool is_slave) {

    // Setup SPI peripheral and GPIO pins
    spi_init(OPUS_SPI_PORT, OPUS_BAUDRATE);
    gpio_init_mask(OPUS_SPI_PINS);
    gpio_set_function(pOPUS_SPI_SCK, GPIO_FUNC_SPI);
    gpio_set_function(pOPUS_SPI_MISO, GPIO_FUNC_SPI);
    gpio_set_function(pOPUS_SPI_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(pOPUS_SPI_CS, GPIO_FUNC_SPI);

    // THIS LINE IS ABSOLUTELY KEY. Enables multi-byte transfers with one CS assert
    // Page 537 of the RP2040 Datasheet.
    spi_set_format(OPUS_SPI_PORT, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST); 

    if(is_slave) {
        spi_set_slave(OPUS_SPI_PORT, true);
        gpio_set_dir(pOPUS_SPI_MISO, GPIO_OUT);
    } else {
        gpio_set_dir(pOPUS_SPI_SCK, GPIO_OUT);
        gpio_set_dir(pOPUS_SPI_MOSI, GPIO_OUT);
        gpio_set_dir(pOPUS_SPI_CS, GPIO_OUT);
    }


    // Setup DMA
    spi_dma_rx = dma_claim_unused_channel(true); // Will panic if no channel!
    dma_channel_config cnfg = dma_channel_get_default_config(spi_dma_rx);
    channel_config_set_transfer_data_size(&cnfg, DMA_SIZE_8);
    channel_config_set_dreq(&cnfg, spi_get_dreq(OPUS_SPI_PORT, false));
    channel_config_set_read_increment(&cnfg, false);
    channel_config_set_write_increment(&cnfg, true); 
    // channel_config_set_ring(&cnfg, true, 8);
    channel_config_set_irq_quiet(&cnfg, false); // irq on every transfer

    dma_irqn_set_channel_enabled(0, spi_dma_rx, true);


    dma_channel_configure(spi_dma_rx,
                          &cnfg, 
                          spi_incoming_packet.buf,
                          &spi_get_hw(OPUS_SPI_PORT)->dr,
                          sizeof(opus_packet_t),
                          true); // start


    // Setup DMA IRQ
    irq_set_enabled(DMA_IRQ_0, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_irq); 

    // Initialize Sempahore
    sem_init(&sem_spi_rx, 0, 1);
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

void parse_packet(){ 
    opus_packet_t* pkt = &spi_incoming_packet.rx_packet;

    // do crc check. 

    switch(pkt->type){ 
        case PKT_TYPE_INIT:
            // set the thing to the ready state. 
            break;
        case PKT_TYPE_HEARTBEAT:
            // reset the "watchdog" that will trigger a shutdown of the motors 
            break;
        case PKT_TYPE_VEL: 
            // send PWM data to motors
            break;
        case PKT_TYPE_ENC:
            // send back the accumulated encoder values
            break;
        case PKT_TYPE_STATE:
            // idk
            break;
    }

    spi_write_blocking(OPUS_SPI_PORT, spi_incoming_packet.buf, sizeof(opus_packet_t));

    // once we're done with the packet, re-activate the DMA!
    dma_channel_set_write_addr(spi_dma_rx, spi_incoming_packet.buf, true);
}