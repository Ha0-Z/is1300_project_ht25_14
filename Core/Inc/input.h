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

#endif /* INC_INPUT_H_ */
