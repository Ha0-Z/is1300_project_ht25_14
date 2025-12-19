/*
 * task5.c
 *
 *  Created on: Dec 18, 2025
 *      Author: Hao Zhang
 */

#include "task5.h"
#include "config.h"
#include "usart.h"

// Command: [ID, 0x00, ValHigh, ValLow]

#define CMD_LENGTH 4

void task5_poller(void) {
  uint8_t RxData[4]; // Holds Byte 1 (Message ID)

  // 1. Try to receive the 1st byte (Message ID) with a short timeout
  if (HAL_UART_Receive(&huart2, &RxData, 4, 10) == HAL_OK) {
	  if( RxData[1] != 0) {
		  return;
	  }
	  HAL_UART_Transmit(&huart2, &RxData, 4, 10);
	  uint16_t data = (RxData[2] << 8) | RxData[3];

	  bool success = config_set_value((RxData[0]), data);

	  // Send ACK: 0x01 (Accepted) or 0x00 (Rejected)
	  uint8_t ack = success ? 0x01 : 0x00;
	  HAL_UART_Transmit(&huart2, &ack, 1, 100);
  }
}

