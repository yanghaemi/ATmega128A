// uart.c

#include "define.h"
#include <avr/io.h>
#include "uart.h"

volatile char receive_buffer = 0;

void usart_init()
{

	UCSR0A = 0X00;
	UCSR0B = 0b00011000;	// UCSR0B |= (1 << RXEN0) | (1 << TXEN0);					// RX 수신 TX 송신 허용, (인터럽트 수신 하려면 (1<<RXCIE0) 추가)
	
	UCSR0C = 0b00100110;	//UCSR0C |= (1 << UPM01) | (1 << UPM00) | (1 << UCSZ01) | (1 << UCSZ00); // 비동기 통신, ODD PARITY, 8bit 데이터
							//UCSR0C &= ~(1 << USBS0);											   // STOP BIT = 1
	
	
	unsigned int ubrr = (F_CPU / 16 / BAUD) - 1;						   //(16,000,000 / (16 * 9600)) - 1
	UBRR0H = ubrr >> 8;
	UBRR0L = (uint8_t)ubrr;
}

char uart_receive(char *data)
{
	if (UCSR0A & (1 << RXC0))
	{
		*data = UDR0;
		return 1;
	}
	return 0;	// Polling 형식 uart receive
}

void uart_send(char data)
{
	while (!(UCSR0A & (1 << UDRE0)))
		; // 송신 대기
	UDR0 = data;
}

void uart_flush()
{
	unsigned char dummy;
	while (UCSR0A & (1 << RXC0))
		dummy = UDR0;
}

/*ISR(USART0_RX_vect){
	receive_buffer = UDR0;
}*/	// 인터럽트 수신 방식
