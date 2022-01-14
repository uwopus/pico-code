#include "opus_pwm.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"
#include "opus_comms.h"
#include "opus_encoder.h"
#include "opus_shared_definitions.h"

int main() {
    comms_init(true);
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    while(true){
        // send_packet(ENC, &data, 10);
        // for(int i = 0; i < 10; i++){
        //     data[i]++;
        // }
        // receive_data(&data[0], 8);s
        if(sem_acquire_timeout_ms(&comm_pkt_available_sem, 1)){
            uint8_t arr_index = 0;
            while(spi0_hw->sr & SPI_SSPSR_RNE_BITS){ 
                data[arr_index++] = (spi0_hw->dr);
                sleep_ms(1);
            }

            hw_set_bits(spi0_hw->imsc, SPI_SSPIMSC_RXIM_BITS);
        }

        // gpio_xor_mask(1 << PICO_DEFAULT_LED_PIN);

        sleep_ms(50);
    }     
}