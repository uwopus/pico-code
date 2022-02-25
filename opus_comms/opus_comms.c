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


// uint spi_dma_rx;

union {
    uint8_t buf[sizeof(opus_packet_t)];
    opus_packet_t rx_packet;
} spi_incoming_packet;

semaphore_t sem_spi_rx;

// void dma_irq() {
//     sem_release(&sem_spi_rx);
//     dma_irqn_acknowledge_channel(0, spi_dma_rx);
//     irq_clear(DMA_IRQ_0);
// }
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


    /* *** START OF DMA SXN 
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

    *** END OF DMA SXN */
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

void recieve_packet(){
    spi_read_blocking(OPUS_SPI_PORT, 0, spi_incoming_packet.buf, sizeof(opus_packet_t));
    gpio_put(8, 0);
}



void parse_packet(){ 
    opus_packet_t* inpkt = &spi_incoming_packet.rx_packet;

    union {
        opus_packet_t pkt;
        uint8_t buf[sizeof(opus_packet_t)];
    } returned_packet; 

    uint8_t calculated_crc = crc8(spi_incoming_packet.buf, sizeof(opus_packet_t));

    if(calculated_crc != 0){
       return;
    }
    returned_packet.pkt.t_ms = to_ms_since_boot(get_absolute_time());
    returned_packet.pkt.type = PKT_TYPE_ACK;
    returned_packet.pkt.data[0] = inpkt->type;
    returned_packet.pkt.data[1] = 1; // could be true/false for ACK/NACK. If needed.
    returned_packet.pkt.len = 1;

    int32_t l_enc_value;
    int32_t r_enc_value;

    controller_t* selected_controller;
    mutex_t* controller_mtx;


    switch(inpkt->type){ 
        case PKT_TYPE_INIT:
            // set the thing to the ready state. 
            break;
        case PKT_TYPE_HEARTBEAT:
            // reset the "watchdog" that will trigger a shutdown of the motors 
            break;
        case PKT_TYPE_SET_VEL: 
            vel_goal_L = LEFT_MTR_POLARITY*get_float_from_bytes(&inpkt->data[0]);
            vel_goal_R = RIGHT_MTR_POLARITY*get_float_from_bytes(&inpkt->data[4]);
            memcpy(&returned_packet.pkt.data[2], &inpkt->data[0], 4);
            memcpy(&returned_packet.pkt.data[6], &inpkt->data[4], 4);
            returned_packet.pkt.len = 10;
            break;
        case PKT_TYPE_GET_VEL:
            get_bytes_from_float(LEFT_MTR_POLARITY * get_cur_vel(LEFT), &returned_packet.pkt.data[2]);
            get_bytes_from_float(RIGHT_MTR_POLARITY * get_cur_vel(RIGHT), &returned_packet.pkt.data[6]);
            returned_packet.pkt.len = 10;
            break;           
        case PKT_TYPE_ENC:
            // send back the accumulated encoder values
            l_enc_value = get_encoder_count(LEFT).ticks;
            r_enc_value = get_encoder_count(RIGHT).ticks;

            int32_to_buf(l_enc_value, &returned_packet.pkt.data[2]);
            int32_to_buf(r_enc_value, &returned_packet.pkt.data[6]);
            returned_packet.pkt.len = 10;          
            break;
        case PKT_TYPE_SET_CONFIG:
        // TODO: This will cause a segfault because we're accessing illegal memory
        // Need to make the packet size larger to accomodate this, but we had issues!
            selected_controller = NULL;
            controller_mtx = NULL;
            if(inpkt->data[0] <= SET_N_L){
                selected_controller = &controller_params_L;
                controller_mtx = &controller_params_L_mtx;
            } else if (inpkt->data[0] <= SET_N_R) {
                selected_controller = &controller_params_R;
                controller_mtx = &controller_params_R_mtx;
            }

            if(selected_controller == NULL) {
                printf("Couldn't find the controller!");
                break;
            }
            else{
                mutex_enter_blocking(controller_mtx);
                if (inpkt->data[0] == SET_P_L || inpkt->data[0] == SET_P_R)
                {
                    selected_controller->P = get_float_from_bytes(&inpkt->data[1]);
                }
                else if (inpkt->data[0] == SET_I_L || inpkt->data[0] == SET_I_R)
                {
                    selected_controller->I = get_float_from_bytes(&inpkt->data[1]);
                }
                else if (inpkt->data[0] == SET_D_L || inpkt->data[0] == SET_D_R)
                {
                    selected_controller->D = get_float_from_bytes(&inpkt->data[1]);
                }
                else if (inpkt->data[0] == SET_N_L || inpkt->data[0] == SET_N_R)
                {
                    selected_controller->N = get_float_from_bytes(&inpkt->data[1]);
                }
                mutex_exit(controller_mtx);
            }
            break;
            
        case PKT_TYPE_STATE:
            mutex_enter_blocking(&PICO_STATE_MTX);
            pico_State = get_picoState_from_bytes(&inpkt->data[0]);
            mutex_exit(&PICO_STATE_MTX);
            break;
    }

    // returned_packet.pkt.len = sizeof(opus_packet_t);

    spi_write_blocking(OPUS_SPI_PORT, returned_packet.buf, sizeof(opus_packet_t));

    // once we're done with the packet, re-activate the DMA!
    // dma_channel_set_write_addr(spi_dma_rx, spi_incoming_packet.buf, true);
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
