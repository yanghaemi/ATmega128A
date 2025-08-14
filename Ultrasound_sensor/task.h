#ifndef TASK_H_
#define TASK_H_

unsigned volatile int task_10us;
unsigned volatile int task_1ms;

unsigned int ultrasound_flag;

volatile unsigned int flag_1ms;

void task_init();

void delay_10us();
void task_us();
void task_ms();

uint16_t measure_time_us();


#endif
