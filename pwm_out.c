from machine import Pin, PWM
from time import sleep

pwm_L = PWM(Pin(28)) // GPIO Pin 28 for PWM_L
pwm_R = PWM(Pin(16)) // GPIO Pin 16 for PWM_R

pwm_L.freq(1000)
pwm_R.freq(1000)

while True:
    for duty in range(65025):
		pwm_L.duty_u16(duty)
		sleep(0.0001)
	for duty in range(65025, 0, -1):
		pwm_L.duty_u16(duty)
		sleep(0.0001)