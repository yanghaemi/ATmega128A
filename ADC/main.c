#include <avr/io.h>
#include <avr/interrupt.h>
#include "define.h"
#include <util/delay.h>
#include "ADC.h"
#include "uart.h"
#include "timer1.h"
#include "External_Interrupt.h";


int main() {
	usart_init();
	ADC_init();
	ExternalInterrupt_init();
	//set_timer1_Fast_PWM(8);
	
	
	DDRB = 0X30;
	DDRA = 0XFF;
	DDRC = 0X80; 
	
	sei();

	
	while(1){
	uint16_t value = ADC_read();
	
	if(value >= 871) PORTA=0xff;
	else PORTA = 0X00;
	
	uart_send(adc_high);
	uart_send(adc_low);
	uart_send('\r');
	uart_send('\n');
	uart_flush();
	
	_delay_ms(1000);
	}
		
}

