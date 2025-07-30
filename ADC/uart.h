/*
 * uart.h
 *
 * Created: 2025-07-25 오후 5:50:02
 *  Author: LED031
 */ 
#ifndef UART_H_
#define UART_H_

void usart_init();
char uart_receive();

void uart_send(char data);

void uart_flush();

#endif