/*
 * ADC.h
 *
 * Created: 2025-07-30 오후 1:22:49
 *  Author: LED031
 */ 


#ifndef ADC_H_
#define ADC_H_

uint8_t adc_low;
uint8_t adc_high;

void ADC_init();
uint16_t ADC_read();

#endif /* ADC_H_ */