#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"

extern volatile  char uart_data;  // Your UART interupt code can place read data here
extern volatile  char flag;

void uart_init(void);

void uart_sendChar(char data);

int uart_receive(void);

void uart_sendStr(const char *data);

void uart_interrupt_init();

void uart_interrupt_handler();


#endif /* UART_H_ */
