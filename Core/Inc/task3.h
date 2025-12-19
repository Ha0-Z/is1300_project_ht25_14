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
void task3_input_update(void);
bool task3_is_vertical_ped_waiting(void);
bool task3_is_horizontal_ped_waiting(void);

#endif /* INC_TASK3_H_ */
