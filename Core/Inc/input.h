/*
 * input.h
 *
 *  Created on: Dec 18, 2025
 *      Author: Hao Zhang
 */

#ifndef INC_INPUT_H_
#define INC_INPUT_H_

#include <stdbool.h>
#include <stdint.h>

// Joystick Directions
typedef struct {
  bool up;     // Button A
  bool down;   // Button B
  bool left;   // Button C
  bool right;  // Button D
  bool center; // Button Center
} JoystickState;

// Read the current state of the joystick
JoystickState input_read_joystick(void);

// Read Pedestrian Light Switch 1 (Active Low)
// Returns true if pressed
bool input_read_pl1(void);

// Read Pedestrian Light Switch 2 (Active Low)
// Returns true if pressed
bool input_read_pl2(void);

// Read Car Presence Switches (TL1-TL4)
// Returns true if active (Active Low assumed)
bool input_read_tl1_car(void);
bool input_read_tl2_car(void);
bool input_read_tl3_car(void);
bool input_read_tl4_car(void);

#endif /* INC_INPUT_H_ */
