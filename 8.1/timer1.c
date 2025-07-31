#include <avr/io.h>
#include <avr/interrupt.h>
#include "define.h"
#include <util/delay.h>


volatile int pitch = DO;
volatile int top = 0;
volatile int pre_volume = 100;
volatile int volume = 100;

volatile int play_mode = 0; // 재생 중이면 1, 정지 or 노래 끝 0
volatile int adc_volume_mode = 0; // adc로 듀티 조절 할 거면 1, 아니면 0
volatile int volume_onoff_mode = 1; // 1: 소리 켜기, 0: 소리 끄기

int *song_pointer = 0;
unsigned int song_len = 0;
volatile int song_idx = 0;


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
const unsigned int candy[72]={
	R_RE,SOL,RA,TI,TI,TI,DO,DO,DO,DO,TI,RA,RA,RA, TI, TI,R_RE,SOL,RA,TI,TI,RA,RA,SOL,SOL,TI,TI,TI,DO,TI,DO,TI,RA,SOL,RA,TI,TI,TI,DO,DO,DO,DO,TI,RA,RA, RA,TI,TI,RA,RA,RA,SOL,FA_,SOL,SOL,FA_,SOL,MI,MI,DO,DO,TI,TI,RA,RA,SOL,SOL,FA_,FA_,SOL,SOL,RA,RA,SOL,SOL,SOL,SOL
};
const unsigned int SCHOOL_BELL[28] = {
	SOL, SOL, RA, RA, SOL, SOL, MI, MI,
	SOL, SOL, MI, MI, RE, RE, RE, RE,
	SOL, SOL, RA, RA, SOL, SOL, MI, MI,
	SOL, SOL, RE, MI, DO, DO, DO, DO
};

void set_pwm_duty(uint16_t percent) {
	
	
	
	if (percent <= 0) {
		//pre_volume = volume;	// 갑자기 음소거하거나 stop 한 경우에 다시 원래 volume으로 돌아가기 위해 이전 볼륨을 저장해놓는다. 
		volume = 0;
		volume_onoff_mode = 0;
		TCCR1A &= ~(1 << COM1A1);	// OC1A 해제 (PWM X)
		TIMSK &= ~(1<<OCIE1A);		// 인터럽트도 꺼줌
		
		PORTB &= ~(1<<5); // 부저 핀 출력 끄기
		PORTA|=0X02; // 디버그 LED 점등
		
		
		return; 
	}
	
	if (percent >= 100){
		volume = 100;
		TCCR1A &= ~(1<<COM1A1);	// OC1A 해제 (PWM X)
		
	}
	else TCCR1A |= (1<<COM1A1);	// OC1A 연결
		
	TIMSK |= (1<<OCIE1A);
	PORTA &= ~0X02;
		
	
	volume_onoff_mode = 1;
	
	uint16_t ocr = top / 100 * percent;	// 일단 듀티 100
		
	OCR1AH = ocr >> 8;
	OCR1AL = (uint8_t)ocr;
	
	
	//OCR1A = ocr;
}

void set_pitch(){
	TCNT1H = 0X00;	// TCNT1가 ICR1보다 높아지는 것을 방지하기 위한 초기화
	TCNT1L = 0X00;
	
	top = (F_CPU/pitch)-1;
	
	ICR1H = top >> 8;
	ICR1L = (uint8_t)top;
}

void set_timer1_CS(int n){
	switch(n){
		case 1: TCCR1B |= (1 << CS10); break;
		case 8:   TCCR1B |= (1 << CS11); break;
		case 64: TCCR1B |= (1 << CS11)| (1 << CS10); break;
		case 256: TCCR1B |= (1 << CS12); break;
		case 1024: TCCR1B |= (1 << CS12) | (1<<CS10); break;
	}
}

void set_timer1_Fast_PWM(int n){
	TCCR1A |= (1<<COM1A1)| (1<<WGM11); // non-inverting mode
	TCCR1B |= (1<<WGM12) | (1<<WGM13);   // Fast PWM (wgm3:0 = 1 1 1 0)
	set_timer1_CS(n);
	
	set_pitch();
	
	TIMSK |= (1 << OCIE1A);
}

void change_pitch(char c){
	
	if (c=='C' || c=='c') {
		pitch = DO;
	}
	else if (c=='D'||c=='d') {
		pitch = RE;
	}
	else if (c=='E'||c=='e'){
		pitch = MI;
	}
	else if (c=='F'||c=='f'){
		pitch = FA;
	}
	else if(c=='G'||c=='g'){
		pitch = H_SOL;
	}
	else if(c=='A'||c=='a'){
		pitch = RA;
	}
	else if(c=='B'||c=='b'){
		pitch = TI;
	}
	else if(c=='1'){
		
		song_pointer = grandpa_song;   // song pointer가 할아버지 시계 배열 가리킴
		song_len = sizeof(grandpa_song)/sizeof(int);
		song_idx = 0;
		
		
		play_mode = 1;
		
		
		return;
	}
	else if (c=='2'){
		
		song_pointer = candy;   // song pointer가 candy 배열 가리킴
		song_len = sizeof(candy)/sizeof(int);
		song_idx = 0;
		
		
		play_mode = 1;
		
		return;
		
	}
	else if (c=='3'){
		
		song_pointer = SCHOOL_BELL;   // song pointer가 학교종 배열 가리킴
		song_len = sizeof(SCHOOL_BELL)/sizeof(int);
		song_idx = 0;
		
		play_mode = 1;
		
		
		return;
	}
	else if (c=='9'){ // volume up
		if(volume + 10 <=100){
			volume += 10;
			set_pwm_duty(volume);
		}
		return;
		
	}
	else if (c=='0'){ // volume down
		if(volume - 10 >= 0){
			volume -= 10;
			set_pwm_duty(volume);
		}
		
		return;
	}
	
	else if (c=='s'){ // stop
		play_mode = 0;
		pre_volume = volume;
		set_pwm_duty(0);
		return;
	}
	else if (c=='p'){ // play
		play_mode = 1;
		
		volume = pre_volume;	// 재생하던 볼륨 다시 갱신
		return;
	}

	else if (c=='8'){ // adc로 볼륨 조절 시작/끄기
		adc_volume_mode^=1;
		
		return;
	}
	else if (c=='.'){ // 볼륨 ON/OFF
		volume_onoff_mode ^= 1;
		return;
	}
	
	set_pitch();
	play_mode = 0;
	
}
ISR(TIMER1_COMPA_vect){
	if(volume_onoff_mode) PORTB=0x20; // dimmer + 부저
	else PORTB &= ~0X20;
}


