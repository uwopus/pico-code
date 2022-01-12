#include "opus_pwm.h"
#include "hardware/pwm.h"
#include "opus_comms.h"

uint8_t data = 0;
int main() {
    comms_init(false);

    while(true){
        receive_data(&data, 1);
        data++;
        send_data(&data, 1);
        sleep_ms(10);
    }    
}