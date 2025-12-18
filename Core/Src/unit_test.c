#include "unit_test.h"
#include "config.h"
#include "input.h"
#include "led_driver.h"
#include "task1.h"
#include "task2.h"
#include "task_config.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

#include "task3.h"

// Simple busy wait to avoid interrupt dependencies during unit testing
// Approx for 80MHz Clock

void Test_program(void) {

  //	test_leds();
  // test_led_driver();
  // test_inputs();
  // test_screen();
  // test_joystick();
  // test_switches();
  // task1();
  // task2();
  // test_uart();
  // test_uart_input();
  // Configuration Tests (Run these once to verify config works)
  // test_config_logic();
  // test_config_uart();

  test_task3();
}

void test_leds(void) {
  LED_Driver_Init();

  HAL_GPIO_TogglePin(USR_LED1_GPIO_Port, USR_LED1_Pin);

  // Pattern: Rolling Test - Light one LED at a time (Ignoring traffic rules)
  // There are 24 bits in the shift register chain (3x 74HC595).
  // We iterate from 0 to 23.

  HAL_GPIO_TogglePin(USR_LED1_GPIO_Port, USR_LED1_Pin);

  // Iterate through all 24 bits
  for (int i = 0; i < 24; i++) {
    uint32_t single_led_bit = (1 << i);

    // This function was added specifically for this low-level debugging
    // You might need to add it to led_driver.h/.c if you used
    // set_raw_bits_impl Or access via the driver struct if we exposed it.
    // Note: In led_driver.c I added set_raw_bits_impl to the struct.
    // Assuming interface is: void (*set_raw_bits)(uint32_t bits);
    if (LED_Driver.set_raw_bits) {
      LED_Driver.set_raw_bits(single_led_bit);
    }

    HAL_Delay(1000);
  }
}

// Diagnostic Input Test
// Maps PL1 -> USR_LED1 (PB2)
// Maps PL2 -> USR_LED2 (PB15)
void test_inputs(void) {
  LED_Driver_Init();

  // Ensure LEDs are outputs (Initialized in main.c/gpio.c)

  while (1) {
    // Check PL1
    if (input_read_pl1()) {
      HAL_GPIO_WritePin(USR_LED1_GPIO_Port, USR_LED1_Pin, GPIO_PIN_SET); // ON
    } else {
      HAL_GPIO_WritePin(USR_LED1_GPIO_Port, USR_LED1_Pin,
                        GPIO_PIN_RESET); // OFF
    }

    // Check PL2
    if (input_read_pl2()) {
      HAL_GPIO_WritePin(USR_LED2_GPIO_Port, USR_LED2_Pin, GPIO_PIN_SET); // ON
    } else {
      HAL_GPIO_WritePin(USR_LED2_GPIO_Port, USR_LED2_Pin,
                        GPIO_PIN_RESET); // OFF
    }

    HAL_Delay(10);
  }
}
test_led_driver() {
  /* USER CODE BEGIN StartDefaultTask */
  LED_Driver_Init();
  uint8_t state = 1;

  /* Infinite loop */
  for (;;) {
    HAL_GPIO_TogglePin(USR_LED1_GPIO_Port, USR_LED1_Pin);

    switch (state) {
    case 0: // All RED
      LED_Driver.set_traffic_signal(MODE_TL1, LIGHT_RED);
      LED_Driver.set_traffic_signal(MODE_TL2, LIGHT_RED);
      LED_Driver.set_traffic_signal(MODE_TL3, LIGHT_RED);
      LED_Driver.set_traffic_signal(MODE_TL4, LIGHT_RED);
      LED_Driver.set_pedestrian_signal(MODE_PL1, PED_LIGHT_RED);
      LED_Driver.set_pedestrian_signal(MODE_PL2, PED_LIGHT_RED);
      break;

    case 1: // TL1, TL2 Green (Car)
      LED_Driver.set_traffic_signal(MODE_TL1, LIGHT_GREEN);
      LED_Driver.set_traffic_signal(MODE_TL2, LIGHT_GREEN);
      LED_Driver.set_traffic_signal(MODE_TL3, LIGHT_RED);
      LED_Driver.set_traffic_signal(MODE_TL4, LIGHT_RED);
      LED_Driver.set_pedestrian_signal(MODE_PL1, PED_LIGHT_RED);
      LED_Driver.set_pedestrian_signal(MODE_PL2, PED_LIGHT_RED);
      break;

    case 2: // TL1, TL2 Yellow
      LED_Driver.set_traffic_signal(MODE_TL1, LIGHT_YELLOW);
      LED_Driver.set_traffic_signal(MODE_TL2, LIGHT_YELLOW);
      LED_Driver.set_traffic_signal(MODE_TL3, LIGHT_RED); // Keep others red
      LED_Driver.set_traffic_signal(MODE_TL4, LIGHT_RED);
      break;

    case 3: // PL1, PL2 Green (Walk)
      LED_Driver.set_traffic_signal(MODE_TL1, LIGHT_RED);
      LED_Driver.set_traffic_signal(MODE_TL2, LIGHT_RED);
      LED_Driver.set_traffic_signal(MODE_TL3, LIGHT_RED);
      LED_Driver.set_traffic_signal(MODE_TL4, LIGHT_RED);
      LED_Driver.set_pedestrian_signal(MODE_PL1, PED_LIGHT_GREEN);
      LED_Driver.set_pedestrian_signal(MODE_PL2, PED_LIGHT_GREEN);
      break;

    default:
      LED_Driver.set_traffic_signal(MODE_TL1, LIGHT_RED);
      LED_Driver.set_traffic_signal(MODE_TL2, LIGHT_RED);
      LED_Driver.set_traffic_signal(MODE_TL3, LIGHT_RED);
      LED_Driver.set_traffic_signal(MODE_TL4, LIGHT_RED);
      LED_Driver.set_pedestrian_signal(MODE_PL1, PED_LIGHT_RED);
      LED_Driver.set_pedestrian_signal(MODE_PL2, PED_LIGHT_RED);
      break;
    }

    LED_Driver.update_leds();

    state++;
    if (state > 3)
      state = 0;
    HAL_Delay(500);
  }
}

void test_screen(void) {
  screen_init();
  screen_clear();
  screen_test_checkerboard();
  HAL_Delay(5000);
}

void test_switches(void) {
  LED_Driver_Init();
  while (1) {
    // TL1 Switch -> Controls TL1 Signal
    if (HAL_GPIO_ReadPin(TL1_Car_GPIO_Port, TL1_Car_Pin) == GPIO_PIN_SET) {
      LED_Driver.set_traffic_signal(MODE_TL1, LIGHT_GREEN);
    } else {
      LED_Driver.set_traffic_signal(MODE_TL1, LIGHT_RED);
    }

    // TL2 Switch -> Controls TL2 Signal
    if (HAL_GPIO_ReadPin(TL2_Car_GPIO_Port, TL2_Car_Pin) == GPIO_PIN_SET) {
      LED_Driver.set_traffic_signal(MODE_TL2, LIGHT_GREEN);
    } else {
      LED_Driver.set_traffic_signal(MODE_TL2, LIGHT_RED);
    }

    // TL3 Switch -> Controls TL3 Signal
    if (input_read_tl3_car()) {
      LED_Driver.set_traffic_signal(MODE_TL3, LIGHT_GREEN);
    } else {
      LED_Driver.set_traffic_signal(MODE_TL3, LIGHT_RED);
    }

    // TL4 Switch -> Controls TL4 Signal
    if (input_read_tl4_car()) {
      LED_Driver.set_traffic_signal(MODE_TL4, LIGHT_GREEN);
    } else {
      LED_Driver.set_traffic_signal(MODE_TL4, LIGHT_RED);
    }

    LED_Driver.update_leds();
    HAL_Delay(50);
  }
}

void test_uart(void) {
  while (1) {
    uint8_t Test[] = "Hello World !!!\r\n"; // Data to send
    HAL_UART_Transmit(&huart2, Test, sizeof(Test),
                      10); // Sending in normal mode
    HAL_Delay(1000);
  }
}

void test_uart_input(void) {
  uint8_t RxData;          // Variable to store the incoming byte [cite: 528]
  uint8_t Test[] = "\r\n"; // Data to send

  while (1) {
    // 1. Wait for 1 byte to arrive (Timeout set to 5 seconds) [cite: 536]
    if (HAL_UART_Receive(&huart2, &RxData, 1, 5000) == HAL_OK) {
      // 2. Echo it back: Send the received byte back to the terminal [cite:
      // 537]
      HAL_UART_Transmit(&huart2, &RxData, 1, 10);
      HAL_UART_Transmit(&huart2, Test, sizeof(Test),
                        10); // Sending in normal mode
    }
  }
}

void test_config_logic(void) {
  char msg[128];

  // Reset defaults
  config_init();

  // 1. Valid Update: Toggle Freq
  // Set to 1000ms
  if (config_set_value(CONFIG_ID_TOGGLE_FREQ, 1000) && g_toggleFreq == 1000) {
    snprintf(msg, sizeof(msg), "[PASS] Valid Toggle Freq set to 1000\r\n");
  } else {
    snprintf(msg, sizeof(msg), "[FAIL] Valid Toggle Freq failed\r\n");
  }
  HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), 100);

  // 2. Invalid Update: Pedestrian Delay too short (must be > Orange Delay 2000)
  // Try set to 1500 (Fail expected)
  if (!config_set_value(CONFIG_ID_PEDESTRIAN_DELAY, 1500)) {
    snprintf(msg, sizeof(msg),
             "[PASS] Invalid Ped Delay rejected (Correct)\r\n");
  } else {
    snprintf(msg, sizeof(msg),
             "[FAIL] Invalid Ped Delay 1500 was accepted!\r\n");
  }
  HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), 100);

  // 3. Valid Update: Pedestrian Delay
  // Set to 5000 (Pass expected)
  if (config_set_value(CONFIG_ID_PEDESTRIAN_DELAY, 5000) &&
      g_pedestrianDelay == 5000) {
    snprintf(msg, sizeof(msg), "[PASS] Valid Ped Delay set to 5000\r\n");
  } else {
    snprintf(msg, sizeof(msg), "[FAIL] Valid Ped Delay failed\r\n");
  }
  HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), 100);

  // 4. Invalid Update: Orange Delay too long
  // Try set Orange to 6000 (Current Ped is 5000, so fail expected)
  if (!config_set_value(CONFIG_ID_ORANGE_DELAY, 6000)) {
    snprintf(msg, sizeof(msg),
             "[PASS] Invalid Orange Delay rejected (Correct)\r\n");
  } else {
    snprintf(msg, sizeof(msg),
             "[FAIL] Invalid Orange Delay 6000 was accepted!\r\n");
  }
  HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), 100);
}

void test_config_uart(void) {
  char msg[128];
  uint32_t last_print = 0;

  // Print Instructions
  snprintf(msg, sizeof(msg),
           "\r\n--- UART Config Test ---\r\nSend 4-byte packets to change "
           "values.\r\n");
  HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), 100);

  while (1) {
    // Poll for config commands
    task_config_poller();

    // Print current values every 2 seconds
    if ((HAL_GetTick() - last_print) > 2000) {
      snprintf(msg, sizeof(msg),
               "TogFreq: %lu, PedDel: %lu, WalkDel: %lu, OrDel: %lu\r\n",
               g_toggleFreq, g_pedestrianDelay, g_walkingDelay, g_orangeDelay);
      HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), 100);
      last_print = HAL_GetTick();
    }

    // Small delay to prevent tight loop starvation if we were using RTOS (not
    // strictly needed here but good practice)
    HAL_Delay(10);
  }
}

void test_task3(void) {
  LED_Driver_Init();
  config_init(); // Ensure configs are loaded

  // Optional: Print start message
  char msg[] = "\r\nStarting Task 3 (Integrated Control)...\r\n";
  HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), 100);

  while (1) {
    // Run Main Task Logic
    task3();

    // Also allow configuration updates during runtime
    task_config_poller();

    HAL_Delay(100); // Small delay to prevent tight loop
  }
}
