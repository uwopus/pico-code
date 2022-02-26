#include "opus_comms.h"
#include "opus_encoder.h"
#include "opus_velocity.h"
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

// Define polarities to account for differences in orientation
// Pico only considers ccw as positive, so comms must handle if that causes upward (+) or downward (-) velocity
#define LEFT_MTR_POLARITY (1)
#define RIGHT_MTR_POLARITY (-1)

#define OPUS_SPI_PINS ((1 << pOPUS_SPI_SCK) | (1 << pOPUS_SPI_MISO) | (1 << pOPUS_SPI_MOSI) | (1 << pOPUS_SPI_CS))

union {
    uint8_t buf[sizeof(opus_pico_rx_packet_t)];
    opus_pico_rx_packet_t rx_packet;
} spi_incoming_packet;

semaphore_t sem_spi_rx;

void comms_init() {

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

    spi_set_slave(OPUS_SPI_PORT, true);
    gpio_set_dir(pOPUS_SPI_MISO, GPIO_OUT);
}

void get_bytes_from_float(float value, uint8_t* buf) {
    union {
        float f;
        uint8_t conv_bytes[sizeof(float)];
    } conv_union;

    conv_union.f = value;
    memcpy(buf, conv_union.conv_bytes, sizeof(float));    
} 


float get_float_from_bytes(uint8_t* bytes) {
    union {
        float f;
        uint8_t conv_bytes[4];
    } conv_union;

    memcpy(conv_union.conv_bytes, bytes, 4);
    return conv_union.f;
} 

picoState_t get_picoState_from_bytes(uint8_t * bytes){
    union {
        picoState_t state;
        uint8_t conv_bytes[1];
    } conv_union;

    memcpy(conv_union.conv_bytes, bytes, sizeof(conv_union));
    return conv_union.state;
}

void int32_to_buf(int32_t value, uint8_t* buf){ 
    union {
        int32_t i; 
        uint8_t buf[4]; 
    } conv_union;

    conv_union.i = value; 
    memcpy(buf, conv_union.buf, 4);
}

void handle_packets(){ 
    // Create TX Packet
    union {
        opus_pico_tx_packet_t pkt;
        uint8_t buf[sizeof(opus_pico_tx_packet_t)];
    } tx_packet; 
    
    tx_packet.pkt.t_ms = to_ms_since_boot(get_absolute_time());
    tx_packet.pkt.L_encd_ticks = get_encoder_count(LEFT).ticks;
    tx_packet.pkt.L_cur_vel = LEFT_MTR_POLARITY * get_cur_vel(LEFT);
    tx_packet.pkt.L_goal_vel = vel_goal_L;
    tx_packet.pkt.R_encd_ticks = get_encoder_count(RIGHT).ticks;
    tx_packet.pkt.R_cur_vel = RIGHT_MTR_POLARITY * get_cur_vel(RIGHT);
    tx_packet.pkt.R_goal_vel = vel_goal_R;
    tx_packet.pkt.state_pad_pad_crc.state = pico_State;
    tx_packet.pkt.state_pad_pad_crc.pad1 = 0xfa;
    tx_packet.pkt.state_pad_pad_crc.pad2 = 0xfa;
    tx_packet.pkt.state_pad_pad_crc.crc = 1; //idk

    // Send tx packet
    spi_read_blocking(OPUS_SPI_PORT, 0, spi_incoming_packet.buf, sizeof(opus_pico_rx_packet_t));
    spi_write_blocking(OPUS_SPI_PORT, tx_packet.buf, sizeof(opus_pico_tx_packet_t));
    // spi_write_read_blocking(OPUS_SPI_PORT,tx_packet.buf,spi_incoming_packet.buf,sizeof(opus_pico_tx_packet_t));

    // Process rx packt
    opus_pico_rx_packet_t* inpkt = &spi_incoming_packet.rx_packet;

    uint8_t calculated_crc = crc8(spi_incoming_packet.buf, sizeof(opus_pico_rx_packet_t));

    // if(calculated_crc != 0){
    //    return;
    // }

    // Parse RX Packet
    vel_goal_L = LEFT_MTR_POLARITY * inpkt->L_vel_cmd;
    vel_goal_R = RIGHT_MTR_POLARITY * inpkt->R_vel_cmd;
    pico_State = inpkt->state_cmd;
}

uint8_t crc8(const void* vptr, int len) {
  const uint8_t *data = vptr;
  unsigned crc = 0;
  int i, j;
  for (j = len; j; j--, data++) {
    crc ^= (*data << 8);
    for(i = 8; i; i--) {
      if (crc & 0x8000)
        crc ^= (0x1070 << 3);
      crc <<= 1;
    }
  }
  return (uint8_t)(crc >> 8);
}
