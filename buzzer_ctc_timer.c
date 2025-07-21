#define F_CPU 16000000UL
#define BAUD 9600

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

unsigned int freq[12] = {1047, 1109, 1175, 1245, 1319, 1397, 1475, 1568, 1661, 1760, 1865, 1976}; // 도~시(반음 포함)
volatile int pitch = 1047; // 현재 계이름
int num =0;

void usart_Init(){
	DDRE = 0X02; // PE 1 출력, PE 0 입력 설정
	
	UCSR0B |= (1<<UCSZ02); // RX 수신 허용
	UCSR0C |= (1<<UMSEL0) | (1<<UPM01) | (1<<UPM00) |(1<<UCSZ01) | (1<<UCSZ00); //비동기 통신, ODD PARITY, 8bit 데이터
	UCSR0C &= ~(1 << USBS0);	// STOP BIT = 1
	
	unsigned int ubrr = (F_CPU/16/BAUD) -1; //(16,000,000 / (16 * 9600)) - 1
	UBRR0H = ubrr >> 8;
	UBRR0L = (uint8_t)ubrr;
}

int uart_receive(){
	while(!(UCSR0A & (1<<RXC0)));	// 수신 대기
	PORTA ^= 0x01;
	return UDR0;	// 받은 문자 반환
}

int main(){
	usart_Init();

	TCCR0 |= (1 << WGM01);		// CTC 모드 1 0
	TCCR0 |= (1 << CS02);		// 64분주
	
	OCR0= (F_CPU/2/64/pitch)-1;	// 0~255  // 주파수 관련
	TIMSK = (1<<OCIE0);
	
	EICRB |= (1<<ISC41);
	EIMSK |= (1<<INT4);
	
	sei();
	
	DDRA=0xff;
	DDRB=0X10; // 버저 레지스터 출력 설정
	
	
	
	while (1) {
		char c = uart_receive();

		if (c=='C') {
			pitch = freq[0];
		}
		else if (c=='D') {
			pitch = freq[2];
		}
		else if (c=='E'){
			pitch = freq[4];
		}
		else if (c=='F'){
			pitch = freq[5];
		}
		else if(c=='G'){
			pitch = freq[7];
		}
		else if(c=='A'){
			pitch = freq[9];
		}
		else if(c=='B'){
			pitch = freq[11];
		}
		OCR0= (F_CPU/2/64/pitch)-1;
	}
	
}

ISR(INT4_vect){
	pitch = freq[++num];
	PORTA++;
	
	if(num==12) num=0;
	
}

ISR(INT5_vect){
	int temp = pitch;
	pitch = 0;
}

ISR(TIMER0_COMP_vect){
	//PORTB = 0x10;
}

