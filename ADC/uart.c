/*
 * uart.c
 *
 * Created: 2025-07-25 오후 5:48:31
 *  Author: LED031
 */ 


#include "define.h"
#include <avr/io.h>

void usart_init(){
	DDRE = 0X02; // PE 1 출력, PE 0 입력 설정
	
	UCSR0B |= (1<<RXEN0) | (1<<TXEN0); // RX 수신 TX 송신 허용
	UCSR0C |= (1<<UPM01) | (1<<UPM00) |(1<<UCSZ01) | (1<<UCSZ00); //비동기 통신, ODD PARITY, 8bit 데이터
	UCSR0C &= ~(1 << USBS0);	// STOP BIT = 1
	
	unsigned int ubrr = (F_CPU/16/BAUD) -1; //(16,000,000 / (16 * 9600)) - 1
	UBRR0H = ubrr >> 8;
	UBRR0L = (uint8_t)ubrr;
	
}

char uart_receive(){
	while(!(UCSR0A & (1<<RXC0)));	// 수신 대기
	return UDR0;	// 받은 문자 반환
}

void uart_send(char data){
	while(!(UCSR0A&(1<<UDRE0))); // 송신 대기
	UDR0 = data;
}

void uart_flush(){
	unsigned char dummy;
	while(UCSR0A & (1<<RXC0)) dummy = UDR0;
}