/*
 * timer1.c
 *
 * Created: 2025-07-30 오후 1:29:07
 *  Author: LED031
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "define.h"
#include <util/delay.h>


volatile int pitch = RE;
volatile int top = 0;
volatile int volume = 100;

volatile int playmode = 1; // 중지면 0

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
	if (percent >= 100) {volume = 100; OCR1A = top;}
	if (percent <= 0) {volume = 0; OCR1A = 0;}
	else{
		uint16_t ocr = top / 100 * percent;
		
		OCR1AH = ocr >> 8;
		OCR1AL = (uint8_t)ocr;
		
	}
	
	//OCR1A = ocr;
}

void set_pitch(){
	TCNT1H = 0X00;	// TCNT1가 ICR1보다 높아지는 것을 방지하기 위한 초기화
	TCNT1L = 0X00;
	
	top = (F_CPU/8/pitch)-1;
	
	ICR1H = top >> 8;
	ICR1L = (uint8_t)top;
}

void play_song(){
	
	playmode =1 ; // 재생 시작
	
	for(int i=0;i<song_len;i++){
		pitch = song_pointer[i];
		set_pitch();
		set_pwm_duty(volume);
		
		if(playmode == 0){
			return;
		}
		
		/*if(UCSR0A & (1<<RXC0)){
			return;
		}*/
		
		_delay_ms(250);
	}
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
	
	set_pwm_duty(1);
	
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
		
		play_song();
		
		ICR1H = 0;
		ICR1L = 0;
		
		return;
	}
	else if (c=='2'){
		
		song_pointer = candy;   // song pointer가 candy 배열 가리킴
		song_len = sizeof(candy)/sizeof(int);
		song_idx = 0;
		
		play_song();
		
		ICR1H = 0;
		ICR1L = 0;
		
		return;
		
	}
	else if (c=='3'){
		
		song_pointer = SCHOOL_BELL;   // song pointer가 학교종 배열 가리킴
		song_len = sizeof(SCHOOL_BELL)/sizeof(int);
		song_idx = 0;
		
		play_song();
		
		ICR1H = 0;
		ICR1L = 0;
		
		return;
	}
	else if (c=='9'){ // volume up
		if(volume + 10 <=100){
			volume += 10;
			set_pwm_duty(volume);
		}
	}
	else if (c=='0'){ // volume up
		if(volume - 10 >= 0){
			volume -= 10;
			set_pwm_duty(volume);
		}
	}
	
	else if (c=='s'){ // stop
		if(playmode == 1) // 재생 중이면
		{
			
		}
	}
	else if (c=='p'){ // play
		if(playmode == 0){	// 중지 중이면
			
		}
	}
	
	set_pitch();
	
}




ISR(TIMER1_COMPA_vect){
	PORTB=0x20; // dimmer + 부저
	
	/*if(UCSR0A & (1<<RXC0)){
		playmode = 0;
	}*/
}
