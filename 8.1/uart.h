#ifndef UART_H_
#define UART_H_


volatile char receive_buffer;

void usart_init();
//char uart_receive();

char uart_receive(char* data);

void uart_send(char data);

void uart_flush();

#endif