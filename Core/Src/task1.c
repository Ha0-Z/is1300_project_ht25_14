/*
 * task1.c
 *
 *  Created on: Dec 18, 2025
 *      Author: Hao Zhang
 */

#include "task1.h"
#include "input.h"
#include "led_driver.h"
#include "main.h"

// Task 1 requirements
// R1.1 Initialization: Ped Red, Car Green
// R1.2 Button -> Indicator toggle (toggleFreq) until Ped Green
// R1.3 Car Red after pedestrianDelay
// R1.4 Ped Green for walkingDelay
// R1.5 Ped Red if Car Green or Orange
// R1.6 Car Green -> Orange (orangeDelay) -> Red -> Orange (orangeDelay) ->
// Green

// Timings (in ms)
#define TOGGLE_FREQ 500
#define PEDESTRIAN_DELAY 4000
#define ORANGE_DELAY 2000
#define WALKING_DELAY 5000

#define DIRECTION 0

typedef enum {
  STATE_IDLE,
  STATE_PRE_TRANSITION,
  STATE_CAR_STOPPING,
  STATE_PED_WALK,
  STATE_CAR_STARTING
} State_t;

// Helper to set both car lights
static void set_car_lights(TrafficLightState state) {
  if (DIRECTION == 1) {
    LED_Driver.set_traffic_signal(MODE_TL1, state);
    LED_Driver.set_traffic_signal(MODE_TL3, state);
  } else {
    LED_Driver.set_traffic_signal(MODE_TL2, state);
    LED_Driver.set_traffic_signal(MODE_TL4, state);
  }
}

// Helper to set pedestrian light
static void set_ped_light(PedestrianLightState state) {
  if (DIRECTION == 1) {
    LED_Driver.set_pedestrian_signal(MODE_PL1, state);
  } else {
    LED_Driver.set_pedestrian_signal(MODE_PL2, state);
  }
}

// Helper to set indicator
static void set_indicator(bool on) {
  if (DIRECTION == 1) {
    LED_Driver.set_pedestrian_indicator(MODE_PL1, on);
  } else {
    LED_Driver.set_pedestrian_indicator(MODE_PL2, on);
  }
}

void task1(void) {
  LED_Driver_Init();

  State_t state = STATE_IDLE;
  uint32_t state_start_time = 0;

  // Variables for toggling
  uint32_t last_toggle_time = 0;
  bool indicator_state = false;

  // R1.1 Initialization
  set_car_lights(LIGHT_GREEN);
  set_ped_light(PED_LIGHT_RED);
  set_indicator(false);
  LED_Driver.update_leds();

  while (1) {
    uint32_t current_time = HAL_GetTick();

    bool button_pressed = false;
    if (DIRECTION == 1) {
      button_pressed = input_read_pl1();
    } else {
      button_pressed = input_read_pl2();
    }

    switch (state) {
    case STATE_IDLE:
      // R1.1 Ensure State (Car Green, Ped Red)
      set_car_lights(LIGHT_GREEN);
      set_ped_light(PED_LIGHT_RED);
      set_indicator(false);

      if (button_pressed) {
        state = STATE_PRE_TRANSITION;
        state_start_time = current_time;
        last_toggle_time = current_time;
        indicator_state = true;
        // Initial toggle ON
        set_indicator(true);
      }
      break;

    case STATE_PRE_TRANSITION:
      // R1.2 Indicator toggles
      if ((current_time - last_toggle_time) >= TOGGLE_FREQ) {
        indicator_state = !indicator_state;
        set_indicator(indicator_state);
        last_toggle_time = current_time;
      }

      if ((current_time - state_start_time) >=
          (PEDESTRIAN_DELAY - ORANGE_DELAY)) {
        state = STATE_CAR_STOPPING;
        state_start_time = current_time;
      }
      break;

    case STATE_CAR_STOPPING:
      // R1.6 Car Green -> Orange
      set_car_lights(LIGHT_YELLOW); // Orange
      set_ped_light(PED_LIGHT_RED);

      // Toggle continues
      if ((current_time - last_toggle_time) >= TOGGLE_FREQ) {
        indicator_state = !indicator_state;
        set_indicator(indicator_state);
        last_toggle_time = current_time;
      }

      if ((current_time - state_start_time) >= ORANGE_DELAY) {
        state = STATE_PED_WALK;
        state_start_time = current_time;
      }
      break;

    case STATE_PED_WALK:
      // R1.5 Car Red, Ped Green
      set_car_lights(LIGHT_RED);
      set_ped_light(PED_LIGHT_GREEN);

      set_indicator(false);

      if ((current_time - state_start_time) >= WALKING_DELAY) {
        state = STATE_CAR_STARTING;
        state_start_time = current_time;
      }
      break;

    case STATE_CAR_STARTING:
      // R1.6 Car Red -> Orange
      set_car_lights(LIGHT_YELLOW); // Orange
      set_ped_light(PED_LIGHT_RED);

      if ((current_time - state_start_time) >= ORANGE_DELAY) {
        state = STATE_IDLE; // Loop back
                            // Car will turn Green in IDLE
      }
      break;
    }

    LED_Driver.update_leds();
    HAL_Delay(10);
  }
}
