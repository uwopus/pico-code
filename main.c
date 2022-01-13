#include "opus_pwm.h"
#include "hardware/pwm.h"
#include "opus_comms.h"
#include "opus_encoder.h"
#include "opus_shared_definitions.h"

uint8_t data = 0;
int main() {
    comms_init(true);

    while(true){
        send_packet(ENC, &data, 1);
        sleep_us(10);
    }     
}