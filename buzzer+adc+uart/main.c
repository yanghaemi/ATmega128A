#include <avr/io.h>
#include <avr/interrupt.h>


#include "ADC.h"
#include "uart.h"
#include "timer1.h"
#include "External_Interrupt.h";
#include "task.h"
 


int main()
{
	usart_init();
	ADC_init();
	// ExternalInterrupt_init();
	set_timer1_Fast_PWM(1);
	task_init();

	DDRB |=(1<<6);
	DDRA = 0XFF;

	sei(); // xc8 에선 실행 안 됨

	while (1)
	{
		
	}
}
