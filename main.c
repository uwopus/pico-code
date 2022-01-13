#include "opus_pwm.h"
#include "hardware/pwm.h"
#include "opus_comms.h"
#include "opus_encoder.h"
#include "opus_shared_definitions.h"

uint8_t data[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
int main() {
    comms_init(false);

    while(true){
        send_packet(ENC, &data, 10);
        for(int i = 0; i < 10; i++){
            data[i]++;
        }
        sleep_ms(10);
    }     
}