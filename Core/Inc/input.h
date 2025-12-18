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

/**
@brief Reads the current state of the 5-way joystick.
@param void
@return JoystickState Struct containing button states.
 */
JoystickState input_read_joystick(void);

/**
@brief Checks if Pedestrian Light Switch 1 is pressed (Active Low).
@param void
@return true if pressed, false otherwise.
 */
bool input_read_pl1(void);

/**
@brief Checks if Pedestrian Light Switch 2 is pressed (Active Low).
@param void
@return true if pressed, false otherwise.
 */
bool input_read_pl2(void);

/**
@brief Checks if Car Sensor TL1 is active.
@param void
@return true if active, false otherwise.
 */
bool input_read_tl1_car(void);

/**
@brief Checks if Car Sensor TL2 is active.
@param void
@return true if active, false otherwise.
 */
bool input_read_tl2_car(void);

/**
@brief Checks if Car Sensor TL3 is active.
@param void
@return true if active, false otherwise.
 */
bool input_read_tl3_car(void);

/**
@brief Checks if Car Sensor TL4 is active.
@param void
@return true if active, false otherwise.
 */
bool input_read_tl4_car(void);

#endif /* INC_INPUT_H_ */
