/*
 * timer1.h
 *
 * Created: 2025-07-30 오후 1:28:50
 *  Author: LED031
 */ 


#ifndef TIMER1_H_
#define TIMER1_H_

volatile int top;
volatile int volume;

const unsigned int grandpa_song[198];
const unsigned int candy[72];
const unsigned int SCHOOL_BELL[28];

void set_pwm_duty(uint16_t percent);
void set_pitch();
void play_song();
void set_timer1_CS(int n);
void set_timer1_Fast_PWM(int n);
void change_pitch(char c);

#endif /* TIMER1_H_ */