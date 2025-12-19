/*
 * task5.c
 *
 *  Created on: Dec 18, 2025
 *      Author: Hao Zhang
 */

#include "task5.h"
#include "config.h"
#include "usart.h"
#include "cmsis_os.h"

#define CMD_LENGTH 4


extern osMessageQueueId_t uartQueueHandle;
extern UART_HandleTypeDef huart2;

// RX Buffer for Interrupt
static uint8_t rx_byte;

// Command Parsing State
static uint8_t cmd_buffer[CMD_LENGTH];
static uint8_t cmd_idx = 0;

// Command: [ID, 0x00, ValHigh, ValLow]


// ISR Callback (Called from HAL_UART_IRQHandler)
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart->Instance == USART2) {
    osMessageQueuePut(uartQueueHandle, &rx_byte, 0, 0); // 0 timeout (ISR)
    HAL_UART_Receive_IT(&huart2, &rx_byte, 1); // Re-arm
  }
}

void StartTask5_Impl(void *argument) {
  uint8_t rx_data;

  // Start Reception
  HAL_UART_Receive_IT(&huart2, &rx_byte, 1);

  for(;;) {
    // Block until byte received
    if (osMessageQueueGet(uartQueueHandle, &rx_data, NULL, osWaitForever) == osOK) {
      
      // Simple Parser
      cmd_buffer[cmd_idx++] = rx_data;

      if (cmd_idx >= CMD_LENGTH) {
        // Process Packet
        if (cmd_buffer[1] == 0) { // Validation
           // Echo (Optional, based on original code)
           HAL_UART_Transmit(&huart2, cmd_buffer, 4, 100);

           uint16_t val = (cmd_buffer[2] << 8) | cmd_buffer[3];
           bool success = config_set_value(cmd_buffer[0], val);

           uint8_t ack = success ? 0x01 : 0x00;
           HAL_UART_Transmit(&huart2, &ack, 1, 100);
        }
        
        // Reset
        cmd_idx = 0;
      }
    }
  }
}

