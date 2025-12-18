/*
 * task_config.c
 *
 *  Created on: Dec 18, 2025
 *      Author: Hao Zhang
 */

#include "task_config.h"
#include "config.h"
#include "usart.h"

// Command Structure
// Byte 1: Message ID
// Byte 2: 0x00
// Byte 3: Val High
// Byte 4: Val Low

#define CMD_LENGTH 4

void task_config_poller(void) {
  uint8_t buffer[CMD_LENGTH];

  // Non-blocking check or blocking check with short timeout
  // Using short timeout (e.g. 1ms or 10ms) to allow this to be called in a loop
  // inside unit_test or RR scheduler.
  // If run in RTOS task, can block longer.

  if (HAL_UART_Receive(&huart2, buffer, CMD_LENGTH, 10) == HAL_OK) {

    // Parse
    uint8_t msg_id = buffer[0];
    // buffer[1] is 0x00 reserved
    uint16_t value = (buffer[2] << 8) | buffer[3]; // Big Endian Assumption

    // Update Config
    bool success = config_set_value(msg_id, value);

    // Send ACK
    // Byte 1: 0x01 (Accepted) or 0x00 (Rejected)
    uint8_t ack = success ? 0x01 : 0x00;
    HAL_UART_Transmit(&huart2, &ack, 1, 100);
  }
}
