/*
 * task_config.c
 *
 *  Created on: Dec 18, 2025
 *      Author: Hao Zhang
 */

#include "task_config.h"
#include "config.h"
#include "usart.h"

// Command: [ID, 0x00, ValHigh, ValLow]

#define CMD_LENGTH 4

void task_config_poller(void) {
  uint8_t buffer[CMD_LENGTH];

  // Short timeout for non-blocking polling behavior

  if (HAL_UART_Receive(&huart2, buffer, CMD_LENGTH, 10) == HAL_OK) {

    // Parse
    uint8_t msg_id = buffer[0];
    // buffer[1] is 0x00 reserved
    uint16_t value = (buffer[2] << 8) | buffer[3]; // Big Endian Assumption

    // Update Config
    bool success = config_set_value(msg_id, value);

    // Send ACK: 0x01 (Accepted) or 0x00 (Rejected)
    uint8_t ack = success ? 0x01 : 0x00;
    HAL_UART_Transmit(&huart2, &ack, 1, 100);
  }
}
