
#include <avr/io.h>
#include <avr/interrupt.h>
#include "define.h"
#include "timer1.h"
#include "External_Interrupt.h"
#include "task.h"
#include "uart.h"

volatile int duty_mode = 1;

volatile uint16_t t_start = 0;  // echo 시작 시간
volatile uint16_t t_end   = 0;  // echo 종료 시간
volatile uint8_t  measuring = 0;   // 시간 측정 flag

void ExternalInterrupt_init()
{
	EICRA |= (1<<ISC21) | (1<<ISC20);	// 상승 edge
	EICRB |= (1<<ISC41);
	EICRB |= (1 << ISC51);
	EIMSK |= (1 << INT5) | (1 << INT4) | (1<<INT2);
}


ISR(INT2_vect){
	
	
	uint8_t rising = (EICRA & (1<<ISC20)) != 0; // 11이면 rising, 10이면 falling

	if (rising) { // 상승엣지: 측정 시작
		//PORTA |= 0x02;
		measuring = 1;
		t_start = task_10us*10;
		// 하강엣지로 변경
		uart_send((t_start)>>8);
		uart_send(t_start);
		EICRA = (EICRA & ~(1<<ISC20)) | (1<<ISC21); // 10: falling
	} 
	else{	// 하강엣지: 측정 종료
	
		if (measuring) {
			t_end = task_10us*10;
		}
		uart_send((t_end)>>8);
		uart_send(t_end);
		
		//PORTA &= ~0x02;
		
		measuring = 0;
		uint16_t t_duration = (((uint32_t)(t_end-t_start))/58);
		if(t_end>=t_start)	{uart_send(t_duration>>8); uart_send(t_duration);}
		// 다시 상승엣지로 변경
		EICRA |= (1<<ISC21)|(1<<ISC20); // 11: rising
	}
}

ISR(INT4_vect){
	//PORTA |= (1<<4);
	ultrasound_flag = WAIT;
}

ISR(INT5_vect)
{ // sw2
	switch (duty_mode)
	{ // timer1 pwm 듀티 조절
	case 1:
		//set_pwm_duty(80);
		volume = 80;
		pre_volume = volume;
		
		duty_mode = 2;
		PORTA = 0x02;
		break;
	case 2:
		//set_pwm_duty(100);
		volume = 100;
		pre_volume = volume;
		
		duty_mode = 1;
		PORTA = 0x01;
		break;
	}
}
