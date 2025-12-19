/*
 * task5.h
 *
 *  Created on: Dec 18, 2025
 *      Author: Hao Zhang
 */

#ifndef INC_task5_H_
#define INC_task5_H_

/**
 @brief Polling function for Task 5.
 @param void
 @return void
 */
// Polling function (now processes buffer)
/**
@brief Polls the ring buffer for valid packets (4 bytes).
@param void
@return void
 */
void task5_poller(void);

/**
@brief Initializes UART reception.
@param void
@return void
 */
void task5_uart_init(void);

#endif /* INC_task5_H_ */
