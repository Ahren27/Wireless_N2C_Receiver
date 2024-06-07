/*
 * uart.c
 *
 *  Created on: Jun 1, 2024
 *      Author: ahren
 */

#include "main.h"
#include "uart.h"

void UART_Init(void) {
	// Configure GPIOA for USART2
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);
	RCC->APB1ENR1 |= (RCC_APB1ENR1_USART2EN);

	// Set Pins 2 and 3 to AF mode
	GPIOA->MODER &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
	GPIOA->MODER |= (GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1);
	GPIOA->AFR[0] &= ~((7 << GPIO_AFRL_AFSEL2_Pos) | 7 << GPIO_AFRL_AFSEL3_Pos);
	GPIOA->AFR[0] |= ((7 << GPIO_AFRL_AFSEL2_Pos) | 7 << GPIO_AFRL_AFSEL3_Pos);

	// Configure USART2
	/* TXEIE	0
	 * TCIE		0
	 * TCIE		0
	 * RXNEIE	1
	 * IDLEIE	0
	 *
	 * TE		1
	 * RE		1
	 * USEM		0
	 * UE		1
	 */
	USART2->CR1 |= (0b000100000);

	// Configure BR
	USART2->BRR = (694);

	// Enable  bits
	USART2->CR1 |= (0b1101);

	// Enable interupts
	NVIC->ISER[1] |= (1 << (USART2_IRQn & 0X1F));
	__enable_irq();

}

// Prints a character to the UART serial connection
void UART_Print_Char(char character) {
	while (!(USART2->ISR & 1 << 7)) {
	};
	USART2->TDR = character;
}

// Prints a character to the UART serial connection and appends a NEWLINE char
void UART_PrintLn(char *output) {
	uint8_t i = 0;
	while (output[i] != '\0') {
		UART_Print_Char(output[i]);
		i++;
	}
	// Finish with new line and return cairage chars
	while (!(USART2->ISR & 1 << 7)) {
	};
	USART2->TDR = '\n';
	//while(!(USART2 -> ISR & 1 << 7)){};
	//USART2 -> TDR = '\r';
}
