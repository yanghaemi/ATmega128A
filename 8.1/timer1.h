#ifndef TIMER1_H_
#define TIMER1_H_

volatile int pitch;
volatile int top;
volatile int pre_volume;
volatile int volume;

volatile int play_mode;
volatile int adc_volume_mode;
volatile int volume_onoff_mode;

int *song_pointer;
unsigned int song_len;
volatile int song_idx;

const unsigned int grandpa_song[198];
const unsigned int candy[72];
const unsigned int SCHOOL_BELL[28];

void set_pwm_duty(uint16_t percent);
void set_pitch();
void set_timer1_CS(int n);
void set_timer1_Fast_PWM(int n);
void change_pitch(char c);

#endif /* TIMER1_H_ */