#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned char digit[10] ={0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7c, 0x07, 0x7f, 0x67};
unsigned char fnd[4];

volatile int count=0;   // 시간

volatile int mode=1; // 1 : 1s , 2: 1ms
volatile int duty_mode = 1; // 1: 100%, 2: 80%, 3: 60%, 4: 40%, 5: 20%


ISR(INT4_vect){   // sw1
	if(mode == 1){ // 1 -> 2 모드 변경
		mode = 2;
		PORTA=0X02;
	}
	else if(mode == 2){   // 2 -> 1 모드 변경
		mode = 1;
		PORTA=0X01;
	}
}

void set_pwm_duty(uint16_t percent) {
	if (percent > 100) percent = 100;
	uint16_t ocr = (uint16_t)(percent * 255 / 100);
	OCR1AL = ocr & 0xFF;
	OCR1AH = ocr >> 8;
}

ISR(INT5_vect){   // sw2
	switch(duty_mode){   // timer1 pwm 듀티 조절
		case 1: set_pwm_duty(80); duty_mode = 2; PORTA++; break;
		case 2: set_pwm_duty(60); duty_mode = 3; PORTA++; break;
		case 3: set_pwm_duty(40); duty_mode = 4; PORTA++; break;
		case 4: set_pwm_duty(20); duty_mode = 5; PORTA++; break;
		case 5: set_pwm_duty(100); duty_mode = 1; PORTA=0x01; break;
	}
}

ISR(TIMER1_COMPA_vect){
	PORTB=0x20;   // dimmer LED light on
}

void reset_timer0(){
	// ----- timer 0 초기화 -----
	TCCR0 = 0;
	TCNT0 = 0;
	OCR0 = 0;
	TIMSK &= ~(1<<OCIE0);
	//---------------------------
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

void set_CS_timer1(int n){
	switch(n){
		case 1: TCCR1B |= (1 << CS10); break;
		case 8:   TCCR1B |= (1 << CS11); break;
		case 64: TCCR1B |= (1 << CS11)| (1 << CS10); break;
		case 256: TCCR1B |= (1 << CS12); break;
		case 1024: TCCR1B |= (1 << CS12) | (1<<CS10); break;
	}
}

void set_timer0_CTC(int n){
	TCCR0 |= (1 << WGM01); // WGM : 10 // CTC 모드
	
	TCNT0 = 0; // reset timer0 count
	
	switch(n){
		case 8:                                // 8분주
		TCCR0 |= (1 << CS00);                  // 001
		break;
		case 32:                              // 32 분주
		TCCR0 |= (1 << CS00)| (1 << CS01);    // 011
		break;
		case 64:                              // 64 분주
		TCCR0 |= (1 << CS02);                 // 100
		break;
		case 128:                              // 128 분주
		TCCR0 |= (1 << CS02) | (1<<CS00);      // 101
		break;
		case 256:                              // 256 분주
		TCCR0 |= (1 << CS02) | (1<<CS01);      // 110
		break;
		case 1024:                             // 1024 분주
		TCCR0 |= (1 << CS02) | (1<<CS01) | (1<<CS00); // 111
		break;
	}
	OCR0 = (F_CPU/n)-1; // 249 // 1ms 마다 인터럽트, 1ms가 최대 (255까지 가능)
	
	TIMSK |= 1 << OCIE0;
}

void set_timer1_CTC(int n){
	
	TCCR1B |= (1 << WGM12);  // WGM : 10 // CTC 모드
	
	set_CS_timer1(n);
	
	int temp = (F_CPU/n)-1;   // 15624
	OCR1AH = temp >> 8;   // 상위 4비트만 할당해줌
	OCR1AL = (uint8_t) temp;
	
	TIMSK |= 1 << OCIE1A;
}

void set_timer0_Fast_PWM(){   // Fast PWM : timer 0 / DIMER LED 세기 duty로 밝기 조절하기
	TCCR0 |= (1 << WGM01) | (1 << WGM00); // Fast PWM 모드 : 11
	TCCR0 |= (1 << CS02); // 64 분주 : 100
	
	OCR0 = 255; // 초기 세팅: 100% 듀티
	
	TIMSK |= 1 << OCIE0;
}

void set_timer0_PC_PWM(){   // Phase Correct Pwm : timer 0 / DIMER LED 세기 duty로 밝기 조절하기
	TCCR0 |= (1 << WGM01);
	
}

void set_timer1_Fast_PWM(int n){
	TCCR1A |= (1<<COM1A1)| (1<<WGM10); // non-inverting mode
	TCCR1B |= (1<<WGM12) ;   // Fast PWM 8-bit
	set_CS_timer1(n);   // 8 분주
	
	set_pwm_duty(100);	// 초기 100 세팅
	
	TIMSK |= 1 << OCIE1A;
}


int main(){
	DDRE = 0x00; // data direction resister E 입력 설정
	DDRA = 0x0f; // ddrA 출력 설정
	DDRB = 0x20; // ddrB의 dimmer 관련 레지스터 출력 설정
	PORTA = 0x01; // mode 1임을 표시하는 led
	DDRC = 0xff; // fnd 관련 ddr 출력 설정
	DDRG = 0x0f; // fnd_sel 관련 ddr 출력 설정
	//---------인터럽트 -------------------
	reset_timer0();
	reset_timer1();
	
	set_timer0_CTC(64);   // 1ms , 64분주
	//set_timer1_CTC(1024); // 1s, 1024 분주
	
	set_timer1_Fast_PWM(8);
	
	EICRB = (1<<ISC41) | (1<<ISC51);   // Falling Edge 트리거 설정
	EIMSK |= 1<< INT4;
	EIMSK |= 1<< INT5;
	
	sei();                     // 전역적으로 인터럽트 발생 허용
	//----------------
	
	while(1){   // mode 1: 100초 타이머 (1s), mode 2: 10초 타이머 (1ms)
		
	}
}
