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

    gpio_init(8);
    gpio_set_dir(8, GPIO_OUT);

    while(true){
        // send_packet(ENC, &data, 10);
        // for(int i = 0; i < 10; i++){
        //     data[i]++;
        // }
        // receive_data(&data[0], 8);s

        gpio_xor_mask(1 << PICO_DEFAULT_LED_PIN);

        gpio_put(8, spi_rx_buf[5] == 0);

        sleep_ms(50);
    }     
}