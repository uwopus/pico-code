
#include "opus_pio_encoder.h"



// the current location of rotation
static int rotationL = 0;
static int rotationR = 0;


// rotary_encoder_A is the pin for the A of the rotary encoder.
// The B of the rotary encoder has to be connected to the next GPIO.
void rotary_encoder_init(uint rotary_encoder_A,uint pio_num)
{
    uint8_t rotary_encoder_B = rotary_encoder_A + 1;
    PIO pio = pio0; // default to pio0
    uint8_t pio_IRQ = PIO0_IRQ_0; // default to PIO0_IRQ_0
    void (*irq_handler_func)(void);
    /* the ampersand is actually optional */
    irq_handler_func = &pio0_irq_handler;
    
    if (pio_num == 1)
    {
        pio = pio1;
        pio_IRQ = PIO1_IRQ_1;
        irq_handler_func = &pio1_irq_handler;
    }
    else if (pio_num > 1)
    {
        printf("PIO number not defined as either 0 or 1");//print, or log problem
    }
    
    // state machine 0
    uint8_t sm = 0;
    // configure the used pins as input with pull up
    pio_gpio_init(pio, rotary_encoder_A);
    gpio_set_pulls(rotary_encoder_A, true, false);
    pio_gpio_init(pio, rotary_encoder_B);
    gpio_set_pulls(rotary_encoder_B, true, false);
    // load the pio program into the pio memory
    uint offset = pio_add_program(pio, &opus_pio_encoder_program);
    // make a sm config
    pio_sm_config c = opus_pio_encoder_program_get_default_config(offset);
    // set the 'in' pins
    sm_config_set_in_pins(&c, rotary_encoder_A);
    // set shift to left: bits shifted by 'in' enter at the least
    // significant bit (LSB), no autopush
    sm_config_set_in_shift(&c, false, false, 0);
    // set the IRQ handler
    irq_set_exclusive_handler(pio_IRQ, irq_handler_func);
    // enable the IRQ
    irq_set_enabled(pio_IRQ, true);
    pio->inte0 = PIO_IRQ0_INTE_SM0_BITS | PIO_IRQ0_INTE_SM1_BITS;
    // init the sm.
    // Note: the program starts after the jump table -> initial_pc = 16
    pio_sm_init(pio, sm, 16, &c);
    // enable the sm
    pio_sm_set_enabled(pio, sm, true);
}


void pio0_irq_handler()
{
    // test if irq 0 was raised
    if (pio0_hw->irq & 1)
    {
        rotationL = rotationL - 1;
    }
    // test if irq 1 was raised
    if (pio0_hw->irq & 2)
    {
        rotationL = rotationL + 1;
    }
    // clear both interrupts
    pio0_hw->irq = 3;
}

void pio1_irq_handler()
{
    // test if irq 0 was raised
    if (pio1_hw->irq & 1)
    {
        rotationR = rotationR - 1;
    }
    // test if irq 1 was raised
    if (pio1_hw->irq & 2)
    {
        rotationR = rotationR + 1;
    }
    // clear both interrupts
    pio1_hw->irq = 3;
}

int main()
{
    //Setup toggle pin
    const uint TOGGLE_PIN_L = 27;
    gpio_init(TOGGLE_PIN_L);
    gpio_set_dir(TOGGLE_PIN_L, GPIO_OUT);
    const uint TOGGLE_PIN_R = 26;
    gpio_init(TOGGLE_PIN_R);
    gpio_set_dir(TOGGLE_PIN_R, GPIO_OUT);

    const uint BUILTIN_LED = 25;
    gpio_init(BUILTIN_LED);
    gpio_set_dir(BUILTIN_LED, GPIO_OUT);

    gpio_put(BUILTIN_LED, 1);

    // needed for printf
    stdio_init_all();
    // the A of the rotary encoder 0 is connected to GPIO 0, B to GPIO 1
    // rotary_encoder_init(0,0);
    // the A of the rotary encoder 1 is connected to GPIO 26, B to GPIO 27
    rotary_encoder_init(2,1);

    // track rotations
    int prev_rotationL = rotationL;
    int prev_rotationR = rotationR;


    // infinite loop to print the current rotation

    while (true)
    {
        if (rotationL != prev_rotationL || rotationR != prev_rotationR)
        {
            printf("%d | %d\n",rotationL,rotationR);
            gpio_xor_mask(((rotationL != prev_rotationL) << TOGGLE_PIN_R) | ((rotationR != prev_rotationR) << TOGGLE_PIN_L));
            prev_rotationL = rotationL;
            prev_rotationR = rotationR;
        }
    }
}