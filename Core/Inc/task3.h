/*
 * task3.h
 *
 *  Created on: Dec 18, 2025
 *      Author: Hao Zhang
 */

#ifndef INC_TASK3_H_
#define INC_TASK3_H_

#include <stdbool.h>

/**
@brief Executes the integrated traffic control state machine (Task 3).
@param void
@return void
 */
void task3(void);
/**
@brief Reads all inputs and updates state variables.
@param void
@return void
 */
void task3_input_update(void);

/**
@brief Checks if vertical pedestrian button was pressed.
@param void
@return true if waiting, false otherwise.
 */
bool task3_is_vertical_ped_waiting(void);

/**
@brief Checks if horizontal pedestrian button was pressed.
@param void
@return true if waiting, false otherwise.
 */
bool task3_is_horizontal_ped_waiting(void);

#endif /* INC_TASK3_H_ */
