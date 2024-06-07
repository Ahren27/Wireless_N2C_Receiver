/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core
#include "uart.h"
#include "dataMeasurements.h"
#include "spsgrf.h"

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
#define NODE_ADDRESS 0x78
#define TARGET_ADDRESS 0x77
#define MAX_PAYLOAD_SIZE 96
#define RECV_TIMEOUT 10000 // in ms (10 seconds)

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TaskHandle_t task1Handler, task2Handler, task3Handler;
SemaphoreHandle_t sendDataSema, radioMutex;
BaseType_t retVal; // used for checking task creation
uint8_t measurments1[6]; // Raw data from I2C Nunchuck 1 (PB8, PB9)
uint8_t measurments2[6]; // Raw data from I2C Nunchuck 2 (PC0, PC1)
volatile SpiritFlagStatus xTxDoneFlag;
volatile SpiritFlagStatus xRxDoneFlag;
volatile uint32_t startTick = 0;
volatile uint8_t rxLen;
volatile uint8_t rxSrcAddr;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* Task function prototypes --------------------------------------------------*/
void Task1(void *argument);
void Task2(void *argument);
void Task3(void *argument);

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_SPI1_Init();

	/* Set up Spirit */
	SPSGRF_Init();

	/* Set source and destination addresses */
	SpiritPktStackSetMyAddress(NODE_ADDRESS);
	SpiritPktStackSetDestinationAddress(TARGET_ADDRESS);

	/* Set up RTOS */

	/* Create the tasks */
	retVal = xTaskCreate(Task1, "Set Up Program", configMINIMAL_STACK_SIZE,
	NULL, osPriorityHigh, &task1Handler);
	if (retVal != pdPASS) {
		while (1)
			;
	} // check if task creation failed

	sendDataSema = xSemaphoreCreateBinary();
	if (sendDataSema == NULL) {
		while (1)
			;
	} // check if binary semaphore creation failed

	radioMutex = xSemaphoreCreateMutex();
	if (radioMutex == NULL) {
		while (1)
			;
	} // check if mutex creation failed

	/* Start scheduler */
	vTaskStartScheduler();

	/* We should never get here as control is now taken by the scheduler */

	/* Infinite loop */
	while (1)
		;
}

/* Define Tasks --------------------------------------------------------------*/

/* Configure Device */
void Task1(void *argument) {
	// Infinite Loop
	for (;;) {
		// Configure Peripherals
		UART_Init();

		retVal = xTaskCreate(Task2, "Receive Data",
		configMINIMAL_STACK_SIZE,
		NULL, osPriorityNormal, &task2Handler);
		if (retVal != pdPASS) {
			while (1)
				;
		} // check if task creation failed

		retVal = xTaskCreate(Task3, "Decode and Print Data",
		configMINIMAL_STACK_SIZE,
		NULL, osPriorityNormal, &task3Handler);
		if (retVal != pdPASS) {
			while (1)
				;
		} // check if task creation failed

		// Task not needed for rest of program duration, so delete it
		vTaskDelete(NULL);
	}
}

/* Transmit Data */
void Task2(void *argument) {
	// Create payload
	uint8_t payload[MAX_PAYLOAD_SIZE] = { 0 };

	// Infinite Loop
	for (;;) {
		// Reset Tx flag
		xRxDoneFlag = S_RESET;

		// Clear payload
		memset(payload, 0, sizeof(payload));

		// Get current time
		startTick = xTaskGetTickCount();

		// Take the mutex before starting the transmission
		if (xSemaphoreTake(radioMutex, portMAX_DELAY) == pdTRUE) {
			SpiritGotoReadyState();
			SPSGRF_StartRx();

			// Wait for at least 10000ms (10 seconds) for receiving any packets
			while (!xRxDoneFlag
					&& (xTaskGetTickCount() - startTick < RECV_TIMEOUT))
				;

			// Get the length of the payload and source address
			rxLen = SPSGRF_GetRxData(payload);
			rxSrcAddr = SpiritPktStackGetReceivedSourceAddress();

			// Release the mutex after transmission
			xSemaphoreGive(radioMutex);
		}

		if (rxSrcAddr == TARGET_ADDRESS) {
			// Assuming the payload format:
			// First 6 bytes are for measurments1
			// Next 6 bytes are for measurments2

			// Ensure payload is at least the size of both measurement arrays
			if (rxLen >= 12) {
				memcpy(measurments1, payload, 6);
				memcpy(measurments2, payload + 6, 6);
			}
		}

		// Wait for 5ms
		vTaskDelay(5 / portTICK_PERIOD_MS);
	}
}

/* Decode and Print Data */
void Task3(void *argument) {
	// Infinite Loop
	for (;;) {
		// Take the mutex before starting the transmission
		if (xSemaphoreTake(radioMutex, portMAX_DELAY) == pdTRUE) {
			// Decode Raw Bytes Measurement into Global Variables
			split_data(measurments1, measurments2);

			// Uploads data to serial port
			print_data();

			// Release the mutex after transmission
			xSemaphoreGive(radioMutex);
		}

		// Wait for 5ms
		vTaskDelay(5 / portTICK_PERIOD_MS);
	}
}

/* Callback to handle external interrupts */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	SpiritIrqs xIrqStatus;

	if (GPIO_Pin != SPIRIT1_GPIO3_Pin) {
		return;
	}

	SpiritIrqGetStatus(&xIrqStatus);
	if (xIrqStatus.IRQ_TX_DATA_SENT) {
		xTxDoneFlag = S_SET;
	}
	if (xIrqStatus.IRQ_RX_DATA_READY) {
		xRxDoneFlag = S_SET;
	}
	if (xIrqStatus.IRQ_RX_DATA_DISC || xIrqStatus.IRQ_RX_TIMEOUT) {
		SpiritCmdStrobeRx();
	}
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1)
			!= HAL_OK) {
		Error_Handler();
	}

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 10;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM17 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	/* USER CODE BEGIN Callback 0 */

	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM17) {
		HAL_IncTick();
	}
	/* USER CODE BEGIN Callback 1 */

	/* USER CODE END Callback 1 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
