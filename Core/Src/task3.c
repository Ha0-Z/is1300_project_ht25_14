/*
 * task3.c
 *
 *  Created on: Dec 18, 2025
 *      Author: Hao Zhang
 */

#include "task3.h"
#include "config.h"
#include "input.h"
#include "led_driver.h"
#include "main.h"

// Integrated Task 3 State Machine

typedef enum {
  STATE_INIT,
  STATE_VERTICAL_PASS,
  STATE_VERTICAL_STOP,
  STATE_V_PED_WALK,
  STATE_HORIZONTAL_PASS,
  STATE_HORIZONTAL_STOP,
  STATE_H_PED_WALK
} SystemState;

static SystemState state = STATE_INIT;
static uint32_t state_start_time = 0;

static bool v_ped_req = false;
static uint32_t v_ped_req_time = 0;
static bool h_ped_req = false;
static uint32_t h_ped_req_time = 0;

static uint32_t last_toggle_time = 0;
static bool indicator_state = false;

static void update_pedestrian_indicator_logic(uint32_t current_time) {
  if ((v_ped_req || h_ped_req) &&
      (current_time - last_toggle_time >= g_toggleFreq)) {
    indicator_state = !indicator_state;
    if (v_ped_req)
      LED_Driver.set_pedestrian_indicator(MODE_PL1, indicator_state);
    if (h_ped_req)
      LED_Driver.set_pedestrian_indicator(MODE_PL2, indicator_state);
    last_toggle_time = current_time;
  }
}

// Check traffic car triggers
static bool check_task2_triggers(bool main_busy, bool cross_busy,
                                 uint32_t current_time,
                                 uint32_t waiter_arrival) {
  if (!main_busy && !cross_busy) {
    if ((current_time - state_start_time) >= g_greenDelay) {
      return true;
    }
  } else if (!main_busy && cross_busy) {
    return true;
  } else if (main_busy && cross_busy) {
    if ((current_time - state_start_time) >= g_redDelayMax) {
      return true;
    }
  }
  return false;
}

void task3(void) {
  uint32_t current_time = HAL_GetTick();
  update_pedestrian_indicator_logic(current_time);

  bool v_busy = input_read_tl1_car() || input_read_tl3_car();
  bool h_busy = input_read_tl2_car() || input_read_tl4_car();

  switch (state) {
  case STATE_INIT:
    LED_Driver.set_traffic_signal(MODE_TL1, LIGHT_GREEN);
    LED_Driver.set_traffic_signal(MODE_TL3, LIGHT_GREEN);
    LED_Driver.set_traffic_signal(MODE_TL2, LIGHT_RED);
    LED_Driver.set_traffic_signal(MODE_TL4, LIGHT_RED);
    LED_Driver.set_pedestrian_signal(MODE_PL1, PED_LIGHT_RED);
    LED_Driver.set_pedestrian_signal(MODE_PL2, PED_LIGHT_RED);

    state = STATE_VERTICAL_PASS;
    state_start_time = current_time;
    break;

  case STATE_VERTICAL_PASS:

    // Check Input (Only in this state, matching Task 1 behavior)
    if (!v_ped_req && input_read_pl1()) {
      v_ped_req = true;
      v_ped_req_time = current_time;
      indicator_state = true;
      LED_Driver.set_pedestrian_indicator(MODE_PL1, true);
      last_toggle_time = current_time;
    }
    // Check if pedestrian delay has passed
    if (v_ped_req && (current_time - v_ped_req_time >=
                      (g_pedestrianDelay - g_orangeDelay))) {
      state = STATE_VERTICAL_STOP;
      state_start_time = current_time;
      break;
    }

    // Check standard triggers
    if (check_task2_triggers(v_busy, h_busy, current_time, 0)) {
      state = STATE_VERTICAL_STOP;
      state_start_time = current_time;
    }
    break;

  case STATE_VERTICAL_STOP:
    LED_Driver.set_traffic_signal(MODE_TL1, LIGHT_YELLOW);
    LED_Driver.set_traffic_signal(MODE_TL3, LIGHT_YELLOW);

    if ((current_time - state_start_time) >= g_orangeDelay) {
      if (v_ped_req) {
        state = STATE_V_PED_WALK;
      } else {
        state = STATE_HORIZONTAL_PASS;
        LED_Driver.set_traffic_signal(MODE_TL1, LIGHT_RED);
        LED_Driver.set_traffic_signal(MODE_TL3, LIGHT_RED);
        LED_Driver.set_traffic_signal(MODE_TL2, LIGHT_GREEN);
        LED_Driver.set_traffic_signal(MODE_TL4, LIGHT_GREEN);
      }
      state_start_time = current_time;
    }
    break;

  case STATE_V_PED_WALK:
    LED_Driver.set_traffic_signal(MODE_TL1, LIGHT_RED);
    LED_Driver.set_traffic_signal(MODE_TL3, LIGHT_RED);
    LED_Driver.set_pedestrian_signal(MODE_PL1, PED_LIGHT_GREEN);
    LED_Driver.set_pedestrian_indicator(MODE_PL1, false);

    if ((current_time - state_start_time) >= g_walkingDelay) {
      LED_Driver.set_pedestrian_signal(MODE_PL1, PED_LIGHT_RED);
      v_ped_req = false;

      state = STATE_HORIZONTAL_PASS;
      state_start_time = current_time;

      LED_Driver.set_traffic_signal(MODE_TL2, LIGHT_GREEN);
      LED_Driver.set_traffic_signal(MODE_TL4, LIGHT_GREEN);
    }
    break;

  case STATE_HORIZONTAL_PASS:
    if (!h_ped_req && input_read_pl2()) {
      h_ped_req = true;
      h_ped_req_time = current_time;
      indicator_state = true;
      LED_Driver.set_pedestrian_indicator(MODE_PL2, true);
      last_toggle_time = current_time;
    }

    if (h_ped_req && (current_time - h_ped_req_time >=
                      (g_pedestrianDelay - g_orangeDelay))) {
      state = STATE_HORIZONTAL_STOP;
      state_start_time = current_time;
      break;
    }

    if (check_task2_triggers(h_busy, v_busy, current_time, 0)) {
      state = STATE_HORIZONTAL_STOP;
      state_start_time = current_time;
    }
    break;

  case STATE_HORIZONTAL_STOP:
    LED_Driver.set_traffic_signal(MODE_TL2, LIGHT_YELLOW);
    LED_Driver.set_traffic_signal(MODE_TL4, LIGHT_YELLOW);

    if ((current_time - state_start_time) >= g_orangeDelay) {
      if (h_ped_req) {
        state = STATE_H_PED_WALK;
      } else {
        state = STATE_VERTICAL_PASS;
        LED_Driver.set_traffic_signal(MODE_TL2, LIGHT_RED);
        LED_Driver.set_traffic_signal(MODE_TL4, LIGHT_RED);
        LED_Driver.set_traffic_signal(MODE_TL1, LIGHT_GREEN);
        LED_Driver.set_traffic_signal(MODE_TL3, LIGHT_GREEN);
      }
      state_start_time = current_time;
    }
    break;

  case STATE_H_PED_WALK:
    LED_Driver.set_traffic_signal(MODE_TL2, LIGHT_RED);
    LED_Driver.set_traffic_signal(MODE_TL4, LIGHT_RED);
    LED_Driver.set_pedestrian_signal(MODE_PL2, PED_LIGHT_GREEN);
    LED_Driver.set_pedestrian_indicator(MODE_PL2, false);

    if ((current_time - state_start_time) >= g_walkingDelay) {
      LED_Driver.set_pedestrian_signal(MODE_PL2, PED_LIGHT_RED);
      h_ped_req = false;

      state = STATE_VERTICAL_PASS;
      state_start_time = current_time;

      LED_Driver.set_traffic_signal(MODE_TL1, LIGHT_GREEN);
      LED_Driver.set_traffic_signal(MODE_TL3, LIGHT_GREEN);
    }
    break;
  }

  LED_Driver.update_leds();
}
