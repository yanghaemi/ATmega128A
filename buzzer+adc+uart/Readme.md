# buzzer 프로젝트
## 사용핀
- **PA0-7**: 동작 여부 확인을 위해 사용
- **PF0** : ADC0 채널 사용
- **PE0-1** : UART 통신을 위해 사용
- **PB4** : TIMER 0: 1ms task용
- **PB5** : TIMER1A: TOP=OCR1A FAST PWM 모드 사용
- **PB6** : TIEMR1B: 외부 부저 연결 + 듀티 조절을 OCR1B로 하기 위해 사용

## 기능
- 1. 노래 재생
  2. 도~시 계이름 연주
  3. 재생/중지
  4. 볼륨 조절 (DUTY 10%씩 +-)
  5. 조도에 따라 볼륨 조절 모드
 
## 환경
- atmel studio: ATmega128A 코딩 및 코드 다운
- python(vscode) : pyserial로 uart통신하여 위의 5가지 기능 수행
