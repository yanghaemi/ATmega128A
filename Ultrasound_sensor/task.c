// 1ms 태스크
#include <avr/io.h>
#include <avr/interrupt.h>

#include "define.h"
#include "ADC.h"
#include "uart.h"
#include "timer1.h"
#include "External_Interrupt.h"
#include "task.h"

unsigned volatile int task_1ms = 0; // 1ms
unsigned volatile int task_2ms = 0; // 2ms
// unsigned volatile int task_250ms =0;	// 250ms
unsigned volatile int task_1s = 0; // 10ms
unsigned volatile int task_10us = 0; // 1us

unsigned volatile int task_digit = 0;

volatile unsigned int flag_1ms =0;


unsigned int ultrasound_flag= 0;



unsigned int digit[10]= { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e };
unsigned int fnd[4];

void task_init()
{
	TCCR0 |= (1 << WGM01);	// CTC 모드
	TCCR0 |= (1<< CS01);	 // 64 분주
	
	OCR0 = 19; // -> 1ms
	
	TIMSK |= (1 << OCIE0);
	
	
	//TCCR0 |= (1 << CS02);  // 64 분주
	//OCR0 = 249; // F_CPU / 64 / 1000 - 1 -> 1ms

}

void play_note()
{
	if (play_mode == 1)
	{
		if ((song_idx >= 0) && (song_idx < song_len))
		{
			pitch = song_pointer[song_idx];
			set_pitch();
			set_pwm_duty(volume);
		}
		else
		{
			play_mode = 0;
			pre_volume = volume;

			set_pwm_duty(0);

			song_pointer = 0; // 끝나서 리셋
			song_len = 0;
			song_idx = 0;
		}
	}
}

void delay_10us(unsigned int n){
	int current_time = task_10us;
	while(task_10us-current_time < n);
	
	return;
}

void do_every_10us(){
	
}

void do_every_1ms()	
{
	
	
	
	// 모드 확인용 led 점등---------------------------------------------
	if (play_mode)
		PORTA |= 0x20;
	else
		PORTA &= ~0x20;

	if (adc_volume_mode)
		PORTA |= 0X80;
	else
		PORTA &= ~0X80;

	if (volume_onoff_mode)
		PORTA |= 0x40;
	else
		PORTA &= ~0X40;
	//----------------------------------------------------------------
	// 1ms 마다 uart receive하기
	
	char recv;
	if (uart_receive(&recv))
	{
		change_pitch(recv);
	}
	//----------------------------------------------------------------
	
	if (adc_volume_mode == 1)
	{
		unsigned short value = ADC_read();

		// unsigned short duty = (uint32_t)(value * 100) / 1023;
		uint16_t duty = (uint32_t)value * 100 / 1023;

		set_pwm_duty(duty);
	} // -------------------------------
	
	else
	{
		set_pwm_duty(volume);
	}
	
			
}

void do_every_2ms(){
	
			
	if (adc_volume_mode == 1)
	{
		uart_send(adc_high);
		uart_send(adc_low);
	}
}

void do_every_1s()
{	
	//unsigned short value = ADC_read();

	
	//uart_send(value>>8);
	//uart_send(value);
	
}

void task_us()	// us 단위 태스크
{
	
	if(ultrasound_flag == WAIT) {
		ultrasound_flag = TRUE;
		PORTA |= (1<<4);
	}
	else if (ultrasound_flag){
		ultrasound_flag = FALSE;
		PORTA &= ~(1<<4);
	}
	
	
}

void task_ms(){	// ms 단위 태스크
	
	do_every_1ms();		// 1ms task		
	
	if (task_1ms % 2 == 0) do_every_2ms();
	
	if (task_1ms % 300 == 0)
	{
		if (play_mode == 1)
		{
			song_idx++;
			play_note();
		}
	}
	
	if (task_1ms % 1000 == 0)
	{ // 1초 TASK
		task_1s++;
		do_every_1s();
	}

	/*if (task_10us >= 100000)
	{
		task_10us = 0;
		task_1ms = 0;
		task_1s = 0;
	}*/
}

ISR(TIMER0_COMP_vect)
{	
	task_10us++;	
	task_us();
	PORTA ^= 0x01; // 오실로스코프 측정용 led toggle
		
	if(task_10us % 100 == 0){
		task_1ms++;
		//task_10us=0;
		flag_1ms = 1;
		
	
	}
}
