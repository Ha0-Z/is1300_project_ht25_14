/*
 * screen.c
 *
 *  Created on: Dec 17, 2025
 *      Author: Hao Zhang
 */

#include "screen.h"
#include "main.h"
#include "spi.h"

// External SPI Handle
extern SPI_HandleTypeDef hspi2;

// CS Control Macros
// Assumes Disp_CS is defined in main.h
#define SET_CS HAL_GPIO_WritePin(Disp_CS_GPIO_Port, Disp_CS_Pin, GPIO_PIN_SET)
#define RESET_CS                                                               \
  HAL_GPIO_WritePin(Disp_CS_GPIO_Port, Disp_CS_Pin, GPIO_PIN_RESET)

void screen_reset(void) {
  // Pulse Reset Low then High
  // Ensure Reset is Output
  HAL_GPIO_WritePin(Disp_Reset_GPIO_Port, Disp_Reset_Pin, GPIO_PIN_RESET);
  HAL_Delay(10);
  HAL_GPIO_WritePin(Disp_Reset_GPIO_Port, Disp_Reset_Pin, GPIO_PIN_SET);
}

void screen_send_instruction(uint8_t instruction) {
  SET_CS;
  HAL_Delay(10); // Wait time
  RESET_CS;      // Active Low

  uint8_t buff[3];
  // Protocol from snippet: 0x1F, LowNibble, HighNibble
  buff[0] = 0x1F;
  buff[1] = instruction & 0x0F;
  buff[2] = (instruction >> 4);

  HAL_SPI_Transmit(&hspi2, buff, 3, 10);

  // Explicitly Latch at end
  SET_CS;
}

void screen_char(uint8_t chars) {
  SET_CS;
  HAL_Delay(10);
  RESET_CS;

  uint8_t buff[3];
  // Protocol from snippet: 0x5F, LowNibble, HighNibble
  buff[0] = 0x5F;
  buff[1] = chars & 0x0F;
  buff[2] = (chars >> 4);

  HAL_SPI_Transmit(&hspi2, buff, 3, 10);

  // Explicitly Latch at end
  SET_CS;
}

void screen_init(void) {
  // GPIO Initialization SAFEGUARD
  // Ensure Disp_CS, Disp_Reset, Disp_Data are configured as Outputs
  // This protects against SPI3 conflict overtaking PC11

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // Enable Clocks (Just in case)
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  // Disp_CS (PC11)
  GPIO_InitStruct.Pin = Disp_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Disp_CS_GPIO_Port, &GPIO_InitStruct);

  // Disp_Reset (PB6)
  GPIO_InitStruct.Pin = Disp_Reset_Pin;
  // Mode/Pull same as above
  HAL_GPIO_Init(Disp_Reset_GPIO_Port, &GPIO_InitStruct);

  // Disp_Data (PC9) - If needed, though not used in protocol currently
  GPIO_InitStruct.Pin = Disp_Data_Pin;
  HAL_GPIO_Init(Disp_Data_GPIO_Port, &GPIO_InitStruct);

  // Start High
  SET_CS;
  HAL_Delay(10);

  screen_reset();
  HAL_Delay(10);

  // Initialization Sequence from Snippet
  screen_send_instruction(0x80);
  screen_send_instruction(0x3A);
  screen_send_instruction(0x09);
  screen_send_instruction(0x06);
  screen_send_instruction(0x1E);

  screen_send_instruction(0x39);
  screen_send_instruction(0x1B);
  screen_send_instruction(0x6E);
  screen_send_instruction(0x56);
  screen_send_instruction(0x7A);

  screen_send_instruction(0x38);
  screen_send_instruction(0x0F);
  screen_send_instruction(0x01);

  SET_CS;
}

void screen_clear(void) {
  screen_send_instruction(0x01);
  HAL_Delay(2);
}

void screen_test_checkerboard(void) {
  // Character Display Checkerboard
  screen_send_instruction(0x80); // Go to Line 1 start

  // Write 80 characters (4 lines x 20 chars approx coverage)
  // We send line commands to be safe

  for (int line = 0; line < 4; line++) {
    // Set address
    if (line == 0)
      screen_send_instruction(0x80);
    if (line == 1)
      screen_send_instruction(0xA0);
    if (line == 2)
      screen_send_instruction(0xC0);
    if (line == 3)
      screen_send_instruction(0xE0);

    for (int col = 0; col < 20; col++) {
      // Checkerboard Pattern
      if ((line + col) % 2 == 0) {
        screen_char(0xFF); // Block
      } else {
        screen_char(0x20); // Space
      }
    }
  }
}
