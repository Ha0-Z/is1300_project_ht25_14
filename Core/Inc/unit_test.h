#ifndef INC_UNIT_TEST_H_
#define INC_UNIT_TEST_H_

// Test function to run validation loops (Blocking)
void Test_program(void);

// Individual Test Functions
void test_leds(void);
void test_switches(void);
void test_led_driver(void);
void test_inputs(void);
void test_screen(void);
void test_screen_char(void);
void test_uart(void);
void test_uart_input(void);
void test_config_logic(void);
void test_config_uart(void);

#endif /* INC_UNIT_TEST_H_ */
