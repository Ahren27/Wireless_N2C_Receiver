/*
 * uart.h
 *
 *  Created on: Jun 1, 2024
 *      Author: ahren
 */

#ifndef INC_UART_H_
#define INC_UART_H_

void UART_Init(void);
void UART_Print(char* output);
void UART_Print_Char(char character);
void UART_PrintLn(char* output);
void UART_Print_ESC(char* output);

#endif /* INC_UART_H_ */
