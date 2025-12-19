/*
 * unit_test.h
 *
 *  Created on: Dec 17, 2025
 *      Author: Hao
 */
#ifndef INC_UNIT_TEST_H_
#define INC_UNIT_TEST_H_

// Test function to run validation loops (Blocking)
void Test_program(void);

// Individual Test Functions

/**
@brief Tests the LED functionality with a rolling pattern.
@param void
@return void
 */
void test_leds(void);

/**
@brief Tests the switch inputs mapping to LEDs.
@param void
@return void
 */
void test_switches(void);

/**
@brief Low-level test for the LED Driver.
@param void
@return void
 */
void test_led_driver(void);

/**
@brief Diagnostic test for inputs PL1 and PL2 mapping to USR_LEDs.
@param void
@return void
 */
void test_inputs(void);

/**
@brief Simple UART transmit test.
@param void
@return void
 */
void test_uart(void);

/**
@brief UART echo back the received data.
@param void
@return void
 */
void test_uart_input(void);

/**
@brief Runs the integrated Task 3 loop (Traffic Control).
@param void
@return void
 */
void test_task3(void);

/**
@brief Unit test for Task 5 (Configuration & UART).
@param void
@return void
 */
void test_task5(void);

/**
@brief Test for the direct LED control functions.
@param void
@return void
 */
void test_led_functions(void);

#endif /* INC_UNIT_TEST_H_ */
