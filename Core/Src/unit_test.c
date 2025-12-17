#include "unit_test.h"
#include "led_driver.h"
#include "main.h" // For HAL_Delay

// Simple busy wait to avoid interrupt dependencies during unit testing
// Approx for 80MHz Clock

void Test_program(void) { test_led_driver(); };

void test_led_driver(void) {
  /* USER CODE BEGIN StartDefaultTask */
  LED_Driver_Init();
  uint8_t state = 1;

  /* Infinite loop */
  for (;;) {
    HAL_GPIO_TogglePin(USR_LED1_GPIO_Port, USR_LED1_Pin);

    // Traffic Light Sequence
    // Update LEDs is called at end of loop

    // Safety clear at start of loop iteration
    // LED_Driver.clear_all(); // Optional, but set_ functions overwrite bits so
    // usually not needed if we set everything.

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
