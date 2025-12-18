/*
 * task_config.h
 *
 *  Created on: Dec 18, 2025
 *      Author: Hao Zhang
 */

#ifndef INC_TASK_CONFIG_H_
#define INC_TASK_CONFIG_H_

// This function needs to be called frequently (polled)
// or run in its own RTOS task.
void task_config_poller(void);

#endif /* INC_TASK_CONFIG_H_ */
