/*
 * screen.h
 *
 *  Created on: Dec 17, 2025
 *      Author: Hao Zhang
 */

#ifndef INC_SCREEN_H_
#define INC_SCREEN_H_

#include <stdint.h>

// Initialization function
void screen_init(void);

// Send Instruction (Low Level)
void screen_send_instruction(uint8_t instruction);

// Send Character (Data)
void screen_char(uint8_t chars);

// Clear Display
void screen_clear(void);

// Display a checkerboard pattern
void screen_test_checkerboard(void);

#endif /* INC_SCREEN_H_ */
