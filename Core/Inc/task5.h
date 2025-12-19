/*
 * task5.h
 *
 *  Created on: Dec 18, 2025
 *      Author: Hao Zhang
 */

#ifndef INC_task5_H_
#define INC_task5_H_

/**
 * @brief Polling function for Task 5.
 * @param None
 * @return None
 */
// Polling function (now processes buffer)
void task5_poller(void);

// Initialize UART Interrupts
void task5_uart_init(void);

#endif /* INC_task5_H_ */
