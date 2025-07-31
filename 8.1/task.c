// 1ms 태스크

#include <avr/io.h>
#include <avr/interrupt.h>
#include "define.h"
#include <util/delay.h>
#include "ADC.h"
#include "uart.h"
#include "timer1.h"
#include "External_Interrupt.h";
#include "task.h"

unsigned volatile int task_1ms = 0;		// 1ms
// unsigned volatile int task_250ms =0;	// 250ms
unsigned volatile int task_1s = 0;	// 10ms


void task_init(){
	TCCR0 |= (1<<WGM01);	// CTC 모드
	TCCR0 |= (1<<CS02);		// 64 분주
	
	OCR0 = 249;				// F_CPU / 64 / 1000 - 1 -> 1ms
	
	TIMSK |= (1<<OCIE0);
}

void play_note(){ // 250ms task
	if(play_mode==1){
		if((song_idx >= 0) && (song_idx < song_len)){
			pitch = song_pointer[song_idx];
			set_pitch();
			set_pwm_duty(volume);
			
		}
		else{
			play_mode = 0;
			
			set_pwm_duty(0);
			
			song_pointer=0;	// 끝나서 리셋
			song_len=0;
			song_idx=0;
		}
	}
}


void do_every_1ms(){
	if (play_mode) PORTA |= 0x20;
	else PORTA &= ~0x20;
	
	if (adc_volume_mode) PORTA |= 0X80;
	else PORTA &= ~0X80;
	
	if(volume_onoff_mode) PORTA |= 0x40;
	else PORTA &= ~0X40;
	
	// ------- volume on off 모드 --------------------------
	//if(volume_onoff_mode == 0 || volume_off_flag==1) set_pwm_duty(0);	// volume on
	// -----------------------------------------------------
}

void do_every_5ms(){
	
	// 5ms 마다 uart receive하기
	char recv;
	if(uart_receive(&recv)){
		change_pitch(recv);
	}
	
	// ------- adc volume 모드 ---------
	
	if(adc_volume_mode == 1){
		unsigned short value = ADC_read();
		
		unsigned short duty = (uint32_t)(value * 100) / 1023;
		
		set_pwm_duty(duty);
	} // -------------------------------
	else {	// 일반
		
		// ------- volume on off 모드 --------------------------
		// if(volume_onoff_mode == 0) set_pwm_duty(0);	// volume on
		// -----------------------------------------------------
		
		
		set_pwm_duty(volume);
		
	}
}



void do_every_1s(){
	PORTA^=0x01;
}

void task(){
	if(task_1ms % 300 == 0) {
		if(play_mode == 1){
			if(TCCR1A & (1<<COM1A0) == 0){
				TCCR1A |= (1<<COM1A0);
			}
			song_idx++;
			play_note();
		}
	}
	if(task_1ms % 3 == 0){
		if(adc_volume_mode == 1){
			uart_send(adc_high);
			uart_send(adc_low);
		}
	}
	
	if(task_1ms% 5 == 0){
		
		do_every_5ms();
	}
	
	if(task_1ms % 1000 == 0) {	// 1초 TASK
		task_1s ++;
		do_every_1s();
	}
	
	if(task_1ms >= 1000000) {
		task_1ms = 0;
		task_1s = 0;
	}
}


ISR(TIMER0_COMP_vect){
	task_1ms++;
	
	
	task();
}
