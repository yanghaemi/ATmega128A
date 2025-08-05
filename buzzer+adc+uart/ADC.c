#include <avr/io.h>
#include <avr/interrupt.h>
#include "define.h"
#include "ADC.h"

uint8_t adc_low = 0, adc_high = 0;

void ADC_init()
{
	ADMUX = 0b00000000;
	/*
	ADMUX의 offset : 0x07, 0b00000111 -> 기본 64분주
		ADMUX &= ~(1<<REFS1) | ~(1<<REFS0);										// 7~6BIT: REFS1~0 = 00 ) 기준 전압을 AVCC(5V)로 쓴다. (JKIT-128-1은 AREF만 사용 가능)
		ADMUX &= ~(1<<ADLAR);													// 5BIT: ADLAR = 0 ) 오른쪽 정렬
		ADMUX &= ~(1<<MUX4)| ~(1<<MUX3) | ~(1<<MUX2) | ~(1<<MUX1) | ~(1<<MUX0);	// 4~0BIT: MUX4~0 = 00000 ) ADC0 입력 채널 사용
	*/

	ADCSRA = 0b10000111;
	/*
	ADCSRA의 offset : 0x06, 0b00000110 -> 기본 64분주
		ADCSRA |= (1<<ADEN);	// 7BIT: ADEN = 1 ) ADC 사용함
		ADCSRA &= ~(1<<ADSC);   // 6BIT: ADSC = 0 ) ADC 아직 시작 안 함
		ADCSRA &= ~(1<<ADFR);	// 5BIT: ADFR = 0 ) single conversion mode
		ADCSRA &= ~(1<<ADIF);	// 4BIT: ADIF = 0 ) 아직 ADC를 시작하지 않아서 0으로 정해둠
		ADCRA &= ~(1<<ADIE);	// 3BIT: ADIE = 0 ) ADC 인터럽트 안 씀
		ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);	// 2~0BIT: ADPS2~0 = 1 1 1 ) 128 분주
	*/
}

uint16_t ADC_read()
{
	adc_low = 0;
	adc_high = 0;

	unsigned short value = 0;

	ADCSRA |= (1 << ADSC); // ADC 읽기 시작
	while (!(ADCSRA & (1 << ADIF)))
		; // ADSC가 0이 될 때까지 (ADC 끝날 때까지) 대기

	adc_low = ADCL;
	adc_high = ADCH;

	value = (adc_high << 8) | adc_low;

	// return adc_high;
	return value;
}
