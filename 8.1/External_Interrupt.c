#include <avr/io.h>
#include <avr/interrupt.h>
#include "define.h"

volatile int duty_mode = 1;


void ExternalInterrupt_init(){
	EICRB |= (1<< ISC51);
	EIMSK |= (1<< INT5);
}

ISR(INT5_vect){   // sw2
	switch(duty_mode){   // timer1 pwm 듀티 조절
		case 1: set_pwm_duty(80); duty_mode = 2; PORTA=0x02; break;
		case 2: set_pwm_duty(60); duty_mode = 3; PORTA=0x04; break;
		case 3: set_pwm_duty(40); duty_mode = 4; PORTA=0x08; break;
		case 4: set_pwm_duty(20); duty_mode = 5; PORTA=0x10; break;
		case 5: set_pwm_duty(100); duty_mode = 1; PORTA=0x01; break;
	}
}