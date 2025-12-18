/*
 * task2.c
 *
 *  Created on: Dec 18, 2025
 *      Author: Hao Zhang
 */

#include "task2.h"
#include "input.h"
#include "led_driver.h"
#include "main.h"

// Task 2 requirements
// R2.1 Vert=TL1/3, Horiz=TL2/4
// R2.2 No overlaps
// R2.3 Transitions via Orange (orangeDelay)
// R2.4 Ghost Town: Toggle every greenDelay if NO cars
// R2.5 Green stays if Active cars & No waiters
// R2.6 Forced Yield: Switch if Active & Waiter > redDelayMax
// R2.7 Immediate Yield: Switch if Empty & Waiter
// R2.8 Init: Vert Green, Horiz Red

// Timings (in ms)
#define GREEN_DELAY 10000
#define ORANGE_DELAY 500
#define RED_DELAY_MAX 2000

typedef enum {
  STATE_VERTICAL_PASS,
  STATE_VERTICAL_STOP,
  STATE_HORIZONTAL_PASS,
  STATE_HORIZONTAL_STOP
} State_t;

// Helpers to check car presence
// Returns true if ANY car is on Vertical Lane (TL1 or TL3)
static bool vertical_cars_present(void) {
  return (!input_read_tl1_car() || !input_read_tl3_car());
}

// Returns true if ANY car is on Horizontal Lane (TL2 or TL4)
static bool horizontal_cars_present(void) {
  return (!input_read_tl2_car() || !input_read_tl4_car());
}

// Helpers to set lights
static void set_vertical_lights(TrafficLightState state) {
  LED_Driver.set_traffic_signal(MODE_TL1, state);
  LED_Driver.set_traffic_signal(MODE_TL3, state);
}

static void set_horizontal_lights(TrafficLightState state) {
  LED_Driver.set_traffic_signal(MODE_TL2, state);
  LED_Driver.set_traffic_signal(MODE_TL4, state);
}

void task2(void) {
  LED_Driver_Init();

  State_t state = STATE_VERTICAL_PASS;
  uint32_t state_start_time = HAL_GetTick(); // For greenDelay/Orange logic
  uint32_t waiter_arrival_time = 0;          // Track when other lane arrived
  bool waiting = false;                      // Is other lane waiting?

  // R2.8 Init: Vertical Green, Horizontal Red
  set_vertical_lights(LIGHT_GREEN);
  set_horizontal_lights(LIGHT_RED);
  LED_Driver.update_leds();

  while (1) {
    uint32_t current_time = HAL_GetTick();

    bool v_busy = vertical_cars_present();
    bool h_busy = horizontal_cars_present();

    switch (state) {
    case STATE_VERTICAL_PASS:
      // Current: Vert Green
      set_vertical_lights(LIGHT_GREEN);
      set_horizontal_lights(LIGHT_RED);

      // Track Horizontal Waiters
      if (h_busy) {
        if (!waiting) {
          waiting = true;
          waiter_arrival_time = current_time;
        }
      } else {
        waiting = false; // No one waiting anymore
      }

      bool switch_to_horizontal = false;

      // R2.4 Ghost Town: Switch if BOTH empty & time > greenDelay
      if (!v_busy && !h_busy) {
        if ((current_time - state_start_time) >= GREEN_DELAY) {
          switch_to_horizontal = true;
        }
      }
      // R2.7 Immediate Yield: Vert Empty & Horiz Waiting
      else if (!v_busy && h_busy) {
        switch_to_horizontal = true;
      }
      // R2.6 Forced Yield: Vert Busy & Horiz Waiting longer than Max
      else if (v_busy && h_busy) {
        if ((current_time - waiter_arrival_time) >= RED_DELAY_MAX) {
          switch_to_horizontal = true;
        }
      }

      if (switch_to_horizontal) {
        state = STATE_VERTICAL_STOP;
        state_start_time = current_time;
        waiting = false; // Reset waiting tracking for transition
      }
      break;

    case STATE_VERTICAL_STOP:              // Vert Green -> Red
      set_vertical_lights(LIGHT_YELLOW);   // Orange
      set_horizontal_lights(LIGHT_YELLOW); // Orange (PlantUML says both Orange)

      if ((current_time - state_start_time) >= ORANGE_DELAY) {
        state = STATE_HORIZONTAL_PASS;
        state_start_time = current_time;
        waiting = false; // Reset waiter logic for new cycle
      }
      break;

    case STATE_HORIZONTAL_PASS:
      // Current: Horiz Green
      set_vertical_lights(LIGHT_RED);
      set_horizontal_lights(LIGHT_GREEN);

      // Track Vertical Waiters
      if (v_busy) {
        if (!waiting) {
          waiting = true;
          waiter_arrival_time = current_time;
        }
      } else {
        waiting = false;
      }

      bool switch_to_vertical = false;

      // R2.4 Ghost Town
      if (!h_busy && !v_busy) {
        if ((current_time - state_start_time) >= GREEN_DELAY) {
          switch_to_vertical = true;
        }
      }
      // R2.7 Immediate Yield
      else if (!h_busy && v_busy) {
        switch_to_vertical = true;
      }
      // R2.6 Forced Yield
      else if (h_busy && v_busy) {
        if ((current_time - waiter_arrival_time) >= RED_DELAY_MAX) {
          switch_to_vertical = true;
        }
      }

      if (switch_to_vertical) {
        state = STATE_HORIZONTAL_STOP;
        state_start_time = current_time;
        waiting = false;
      }
      break;

    case STATE_HORIZONTAL_STOP:            // Horiz Green -> Red
      set_vertical_lights(LIGHT_YELLOW);   // Orange
      set_horizontal_lights(LIGHT_YELLOW); // Orange

      if ((current_time - state_start_time) >= ORANGE_DELAY) {
        state = STATE_VERTICAL_PASS;
        state_start_time = current_time;
        waiting = false;
      }
      break;
    }

    LED_Driver.update_leds();
    HAL_Delay(10);
  }
}
