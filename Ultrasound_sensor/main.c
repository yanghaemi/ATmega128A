
#include <avr/io.h>
#include <avr/interrupt.h>

#include "ADC.h"
#include "uart.h"
#include "timer1.h"
#include "External_Interrupt.h";
#include "task.h"
#include "define.h"

int main()
{
	
	usart_init();
	ADC_init();
	ExternalInterrupt_init();
	set_timer1_Fast_PWM(1);
	task_init();

	DDRB |= (1<<6) | (1<<4);
	DDRD &= ~(1<<PIND2);	// 입력
	PORTD &= ~(1<<PIND2);
	
	
	DDRA = 0XFF;
		
	sei();

	while (1)
	{
		if (flag_1ms){
			
			task_ms();
			flag_1ms = 0;
			
		}
		
	}
}

