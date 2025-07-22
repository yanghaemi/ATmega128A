#define F_CPU 16000000UL
#define BAUD 9600
#define SOL 784
#define RA 880
#define TI 988
#define DO 1047
#define DO_ 1109
#define RE 1175
#define RE_ 1245
#define MI 1319
#define FA 1397
#define FA_ 1475
#define H_SOL 1568
#define NONE 125000 // 0 나오게 -> 이거 grandpa_song에 넣으면 이상한 음으로 나옴


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

// 할아버지의 낡은 시계 계이름
unsigned int grandpa_song[198] = {
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

volatile int pitch = NONE; // 현재 계이름



int main(){

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
		
		
		for(int i=0;i<99;i++){
			PORTA++;
			
			pitch = grandpa_song[i];
			OCR0 = (F_CPU/2/64/pitch)-1;
			
			_delay_ms(250);
		}
	}
}


ISR(TIMER0_COMP_vect){
	
		PORTB ^= 0x10;     // 토글
	
}

