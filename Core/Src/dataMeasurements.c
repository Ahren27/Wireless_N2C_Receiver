/*
 * dataMeasurements.c
 *
 *  Created on: Jun 6, 2024
 *      Author: ahren
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataMeasurements.h"
#include "uart.h"

// Global Data Variables
volatile uint8_t stick_x1 = 0;
volatile uint8_t stick_y1 = 0;

volatile uint16_t acc_x1 = 0;
volatile uint16_t acc_y1 = 0;
volatile uint16_t acc_z1 = 0;

volatile uint8_t button_c1 = 0;
volatile uint8_t button_z1 = 0;

volatile uint8_t stick_x2 = 0;
volatile uint8_t stick_y2 = 0;

volatile uint16_t acc_x2 = 0;
volatile uint16_t acc_y2 = 0;
volatile uint16_t acc_z2 = 0;

volatile uint8_t button_c2 = 0;
volatile uint8_t button_z2 = 0;

// Output Strings
char sx_str1[13];
char sy_str1[13];
char ax_str1[13];
char ay_str1[13];
char az_str1[13];
char bc_str1[13];
char bz_str1[13];

// Output Strings
char sx_str2[13];
char sy_str2[13];
char ax_str2[13];
char ay_str2[13];
char az_str2[13];
char bc_str2[13];
char bz_str2[13];

// Decode Raw Bytes Measurement into Global Variables
void split_data(uint8_t measurments1[], uint8_t measurments2[]) {
	// x and y axis from first 2 bytes
	stick_x1 = measurments1[0];
	stick_y1 = measurments1[1];

	// x and y axis from first 2 bytes
	stick_x2 = measurments2[0];
	stick_y2 = measurments2[1];

	// Might want to Reset accelerometer values
	acc_x1 = 0;
	acc_y1 = 0;
	acc_z1 = 0;

	// Might want to Reset accelerometer values
	acc_x2 = 0;
	acc_y2 = 0;
	acc_z2 = 0;

	// higher 8 bits of accelerometer from next 3 bytes
	acc_x1 |= (measurments1[2] << 2);
	acc_y1 |= (measurments1[3] << 2);
	acc_z1 |= (measurments1[4] << 2);

	// higher 8 bits of accelerometer from next 3 bytes
	acc_x2 |= (measurments2[2] << 2);
	acc_y2 |= (measurments2[3] << 2);
	acc_z2 |= (measurments2[4] << 2);

	// lower 2 bits from last byte
	acc_x1 |= (((1 << 2) - 1) & (measurments1[5] >> 3));
	acc_y1 |= (((1 << 2) - 1) & (measurments1[5] >> 5));
	acc_z1 |= (((1 << 2) - 1) & (measurments1[5] >> 7));

	// lower 2 bits from last byte
	acc_x2 |= (((1 << 2) - 1) & (measurments2[5] >> 3));
	acc_y2 |= (((1 << 2) - 1) & (measurments2[5] >> 5));
	acc_z2 |= (((1 << 2) - 1) & (measurments2[5] >> 7));

	// Buttons are last 2 bits of last byte
	button_c1 = (1 & ~(measurments1[5] >> 1));
	button_z1 = (1 & ~(measurments1[5] >> 0));

	// Buttons are last 2 bits of last byte
	button_c2 = (1 & ~(measurments2[5] >> 1));
	button_z2 = (1 & ~(measurments2[5] >> 0));
}

// Uploads data to serial port
void print_data() {
	// Convert Data to Strings
	sprintf(sx_str1, "StickX1:%i", stick_x1);
	sprintf(sy_str1, "StickY1:%i", stick_y1);

	sprintf(ax_str1, "AccX1:%i", acc_x1);
	sprintf(ay_str1, "AccY1:%i", acc_y1);
	sprintf(az_str1, "AccZ1:%i", acc_z1);

	sprintf(bc_str1, "ButtonC1:%i", button_c1);
	sprintf(bz_str1, "ButtonZ1:%i", button_z1);

	// Convert Data to Strings
	sprintf(sx_str2, "StickX2:%i", stick_x2);
	sprintf(sy_str2, "StickY2:%i", stick_y2);

	sprintf(ax_str2, "AccX2:%i", acc_x2);
	sprintf(ay_str2, "AccY2:%i", acc_y2);
	sprintf(az_str2, "AccZ2:%i", acc_z2);

	sprintf(bc_str2, "ButtonC2:%i", button_c2);
	sprintf(bz_str2, "ButtonZ2:%i", button_z2);

	// Print strings to terminal
	UART_PrintLn(sx_str1);
	UART_PrintLn(sy_str1);
	UART_PrintLn(ax_str1);
	UART_PrintLn(ay_str1);
	UART_PrintLn(az_str1);
	UART_PrintLn(bc_str1);
	UART_PrintLn(bz_str1);

	// Print strings to terminal
	UART_PrintLn(sx_str2);
	UART_PrintLn(sy_str2);
	UART_PrintLn(ax_str2);
	UART_PrintLn(ay_str2);
	UART_PrintLn(az_str2);
	UART_PrintLn(bc_str2);
	UART_PrintLn(bz_str2);
}
