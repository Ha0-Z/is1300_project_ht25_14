#include "unit_test.h"
#include "led_driver.h"
#include "main.h" // For HAL_Delay

// Simple busy wait to avoid interrupt dependencies during unit testing
// Approx for 80MHz Clock

void Test_program(void) {
  test_inputs();
  //	test_leds();
  //	test_led_driver();
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
    // PL1 (Switch 5 & 6) is PA15. Configured as Pull-Up in gpio.c.
    // Active State is Low (Pressed = 0).
    if (HAL_GPIO_ReadPin(PL1_Switch_GPIO_Port, PL1_Switch_Pin) ==
        GPIO_PIN_RESET) {
      // PL1 Pressed -> Turn ON all LEDs
      if (LED_Driver.set_raw_bits)
        LED_Driver.set_raw_bits(0xFFFFFF);
    }
    // PL2 (Switch 7 & 8) is PB07. Configured as Pull-Up.
    else if (HAL_GPIO_ReadPin(PL2_Switch_GPIO_Port, PL2_Switch_Pin) ==
             GPIO_PIN_RESET) {
      // PL2 Pressed -> Turn OFF all led
      LED_Driver.clear_all();
    }

    LED_Driver.update_leds();

    HAL_Delay(50); // simple debounce / poll interval
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
