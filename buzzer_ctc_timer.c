#define F_CPU 16000000UL
#define BAUD 9600
#define R_RE 587
#define FA_ 740
#define SOL 784
#define RA 880
#define TI 988
#define DO 1047
#define DO_ 1109
#define RE 1175
#define RE_ 1245
#define MI 1319
#define FA 1397
#define H_SOL 1568
#define NONE 125000 // 0 나오게


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>




// 계이름 + 노래 몇곡 넣기

volatile int pitch = NONE; // 현재 계이름


void usart_Init(){
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
	PORTA |= 0x01;
	return UDR0;	// 받은 문자 반환
}

void uart_send(char data){
	while(!(UCSR0A&(1<<UDRE0))); // 송신 대기
	UDR0 = data;
}

void uart_flush(){
	unsigned char dummy;
	while(UCSR0A & (1<<RXC0)) dummy = UDR0;
	PORTA |= 0x02;
}

void set_OCR0(){
	OCR0= (F_CPU/2/64/pitch)-1;
}

void change_pitch(char c){
	if (c=='C') {
		pitch = DO;
	}
	else if (c=='D') {
		pitch = RE;
	}
	else if (c=='E'){
		pitch = MI;
	}
	else if (c=='F'){
		pitch = FA;
	}
	else if(c=='G'){
		pitch = H_SOL;
	}
	else if(c=='A'){
		pitch = RA;
	}
	else if(c=='B'){
		pitch = TI;
	}
	else if(c=='1'){
		const unsigned int grandpa_song[198] = {
			SOL, SOL, DO, DO, TI, DO, RE, RE, DO, RE, MI, MI, FA, MI, RA, RA, RE, RE, DO, DO, DO, DO,
			TI, TI, RA, TI, DO, DO, DO, DO, DO, DO, SOL, SOL, DO, DO, TI, DO, RE, RE, DO, RE, MI, MI,
			FA, MI, RA, RA, RE, RE, DO, DO, DO, DO, TI, TI, RA, TI, DO, DO, DO, DO, DO, DO, DO, MI,
			SOL, SOL, MI, RE, DO, DO, TI, DO, RE, DO, TI, RA, SOL, SOL, DO, MI, SOL, SOL, MI, RE, DO,
			DO, TI, DO, RE, RE, RE, RE, RE, RE, RE, SOL, SOL, DO, DO, DO, DO, RE, RE, RE, RE, MI, MI,
			FA, MI, RA, RA, RE, RE, DO, DO, DO, DO, TI, TI, RA, SOL, DO, DO, DO, DO, DO, DO, DO, SOL,
			SOL, DO, DO, TI, DO, RE, RE, DO, RE, MI, MI, FA, MI, RA, RA, RE, RE, DO, DO, DO, DO, TI,
			TI, RA, TI, DO, DO, DO, DO, DO, DO, SOL, SOL, DO, DO, SOL, SOL, RA, RA, SOL, SOL, MI, SOL,
			MI, SOL, SOL, DO, DO, SOL, SOL, RA, RA, SOL, SOL, MI, MI, SOL, SOL, MI, MI, SOL, SOL, DO,
			DO, DO, DO, RE, RE, RE, RE, MI, MI, FA, RA, RA, RE, RE, DO, DO, DO, DO, TI, TI, RA, TI, DO,
			DO, DO, DO, DO, DO, MI, MI, FA, FA, SOL, SOL, FA, FA, MI, MI, MI, MI, MI, MI
		};
		for(int i =0;i<(sizeof(grandpa_song)/sizeof(int));i++){
			pitch = grandpa_song[i];
			set_OCR0();
			
			_delay_ms(250);
		}
		
		OCR0 = 0;
		
		
		return;
	}
	else if (c=='2'){
		const unsigned int candy[72]={
			R_RE,SOL,RA,TI,TI,TI,DO,DO,DO,DO,TI,RA,RA,RA, TI, TI,R_RE,SOL,RA,TI,TI,RA,RA,SOL,SOL,TI,TI,TI,DO,TI,DO,TI,RA,SOL,RA,TI,TI,TI,DO,DO,DO,DO,TI,RA,RA, RA,TI,TI,RA,RA,RA,SOL,FA_,SOL,SOL,FA_,SOL,MI,MI,DO,DO,TI,TI,RA,RA,SOL,SOL,FA_,FA_,SOL,SOL,RA,RA,SOL,SOL,SOL,SOL
		};
		for(int i =0;i < (sizeof(candy)/sizeof(int));i++){
			pitch = candy[i];
			set_OCR0();
			
			_delay_ms(250);
		}
		
		OCR0 = 0;
		
		
		return;
		
	}
	else if (c=='3'){
		const unsigned int SCHOOL_BELL[28] = {
			SOL, SOL, MI, MI, SOL, SOL, MI,
			SOL, SOL, FA, FA, MI, MI, RE,
			SOL, SOL, MI, MI, SOL, SOL, MI,
			SOL, SOL, FA, FA, MI, MI, RE
		};
		
		for(int i =0;i<(sizeof(SCHOOL_BELL)/sizeof(int));i++){
			pitch = SCHOOL_BELL[i];
			set_OCR0();
			
			_delay_ms(250);
		}
		
		OCR0 = 0;
		
		return;
	}
	set_OCR0();
}



int main(){
	usart_Init();

	TCCR0 |= (1 << WGM01);		// CTC 모드 1 0
	TCCR0 |= (1 << CS02);		// 64분주
	
	OCR0=0;
	TIMSK = (1<<OCIE0);
	
	
	EICRB |= (1<<ISC41);
	EIMSK |= (1<<INT4);
	
	sei();
	
	DDRA=0xff;
	DDRB=0X10; // 버저 레지스터 출력 설정
	

	while(1){
		char c = uart_receive();
		change_pitch(c);
		uart_flush();
	}
}


ISR(TIMER0_COMP_vect){
	PORTB ^= 0x10;     // 토글
}


ISR(INT4_vect){
	DDRB ^= 0x10;
}

