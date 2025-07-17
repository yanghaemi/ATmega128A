/* sw1로 fast PWM과 Phase Correct PWM를 변경할 수 있다.

  7/17 TASK:
- [v] 듀티 87.5% 지정
- [v] 주파수 35Hz로 지정
- [ ] 듀티 파형 왼쪽 정렬, 오른쪽 정렬, 가운데 정렬 하기
*/

#define F_CPU 16000000UL
#define F_PCPWM 35  // phase correct PWM 주파수
#define F_FPWM 35  // Fast PWM 주파수

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile int top = 0; 
volatile int mode=1; // 1 : fast pwm , 2: pc pwm
volatile int duty_mode = 1; // 1: 100%, 2: 80%, 3: 60%, 4: 40%, 5: 20%



void set_pwm_duty(volatile uint16_t percent) {
	//volatile uint16_t temp_percent = percent;  // 디버깅용
	if (percent > 100) percent = 100;
	uint16_t ocr = (percent * top) / 100;
	
	OCR1AH = ocr >> 8;
	OCR1AL = (uint8_t)ocr;
	
	//OCR1A = ocr;
}


void reset_timer1(){
	//----- timer 1 초기화 -----
	TCCR1A = 0x00;
	TCCR1B = 0x00;
	TCNT1 = 0;
	OCR1A = 0;
	OCR1B = 0;
	ICR1 = 0;
	TIMSK &= ~(1<<OCIE1A);
	TIMSK &= ~(1<<OCIE1B);
	
	//--------------------------
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

void set_timer1_CTC(int n){
	
	TCCR1B |= (1 << WGM12);  // WGM : 10 // CTC 모드
	
	set_timer1_CS(n);
	
	int temp = (F_CPU/n)-1;   // 15624
	OCR1AH = temp >> 8;   // 상위 4비트만 할당해줌
	OCR1AL = (uint8_t) temp;
	
	TIMSK |= 1 << OCIE1A;
}

void set_timer1_Fast_PWM(int n){
	TCCR1A |= (1<<COM1A1)| (1<<WGM11); // non-inverting mode
	TCCR1B |= (1<<WGM12) | (1<<WGM13);   // Fast PWM (wgm3:0 = 1 1 1 0)
	set_timer1_CS(n);
	
	top = (F_CPU/n/F_FPWM)-1;
	
	ICR1H = top >> 8;
	ICR1L = (uint8_t)top;	// 주파수를 지정해주기 위한 top 계산
	
	TIMSK |= 1 << OCIE1A;
	
	duty_mode=1;
	set_pwm_duty(100);
	PORTA = 0x01;
	
}

void set_timer1_PC_PWM(int n){
	TCCR1A |= (1<<COM1A1)| (1<<WGM11); // non-inverting mode
	TCCR1B |= (1<<WGM13);   // phase correct PWM (wgm3:0 = 1 0 1 0)
	set_timer1_CS(n);
	
	top = (F_CPU/n/F_PCPWM/2);
	
	ICR1H = top >> 8;
	ICR1L = (uint8_t)top;
	
	TIMSK |= 1 << OCIE1A;
	
	duty_mode=1;
	set_pwm_duty(100);
	PORTA = 0x01;
	
}


int main(){
	DDRE = 0x00; // data direction resister E 입력 설정
	DDRA = 0xff; // ddrA 출력 설정
	DDRB = 0x20; // ddrB의 dimmer 관련 레지스터 출력 설정
	PORTA = 0x01; // mode 1임을 표시하는 led
	//---------인터럽트 -------------------
	reset_timer1();

	set_timer1_Fast_PWM(1024);
	
	
	
	EICRB = (1<<ISC41) | (1<<ISC51);   // Falling Edge 트리거 설정
	EIMSK |= 1<< INT4;
	EIMSK |= 1<< INT5;
	
	sei();                     // 전역적으로 인터럽트 발생 허용
	//----------------
	
	while(1){  
		
	}
}

ISR(INT4_vect){   // sw1
	if(mode == 1){ // 1 -> 2 모드 변경
		mode = 2;
		reset_timer1();
		set_timer1_PC_PWM(1024);
	}
	else if(mode == 2){   // 2 -> 1 모드 변경
		mode = 1;
		reset_timer1();
		set_timer1_Fast_PWM(1024);
	}
}

ISR(INT5_vect){   // sw2
	switch(duty_mode){   // timer1 pwm 듀티 조절
		case 1: set_pwm_duty(80); duty_mode = 2; PORTA=0x02; break;
		case 2: set_pwm_duty(60); duty_mode = 3; PORTA=0x04; break;
		case 3: set_pwm_duty(40); duty_mode = 4; PORTA=0x08; break;
		case 4: set_pwm_duty(20); duty_mode = 5; PORTA=0x10; break;
		case 5: set_pwm_duty(100); duty_mode = 1; PORTA=0x01; break;
	}
	
}


ISR(TIMER1_COMPA_vect){
	PORTB=0x20;   // dimmer LED light on
	
	if(mode == 1){ // fast PWM 오실로스코프 듀티 정렬 확인용
		PORTA^=0x80;
		PORTA&=0b10111111;
	}
	else if(mode == 2){	// pc pwm 오실로스코프 듀티 정렬 확인용
		PORTA^=0b01000000;
		PORTA&=0b01111111;
	}
}
