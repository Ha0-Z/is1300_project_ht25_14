#include "unit_test.h"
#include "input.h"
#include "led_driver.h"
#include "main.h" // For HAL_Delay
#include "screen.h"

// Simple busy wait to avoid interrupt dependencies during unit testing
// Approx for 80MHz Clock

void Test_program(void) {

  //	test_leds();
  //test_led_driver();
   // test_inputs();
  // test_screen();
  // test_joystick();
  test_switches();
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

void test_inputs(void) {
  LED_Driver_Init();

  // Safety clear initially
  LED_Driver.clear_all();
  LED_Driver.update_leds();

  while (1) {
    // Check PL1 via Input Driver
    if (input_read_pl1()) {
      LED_Driver.set_raw_bits(0xFFFFFF); // All On
    }
    // Check PL2 via Input Driver
    else if (input_read_pl2()) {
      LED_Driver.set_raw_bits(0x000000); // All Off
    } else {
      // Hold state
    }
    LED_Driver.update_leds();
    HAL_Delay(50); // Faster polling
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
