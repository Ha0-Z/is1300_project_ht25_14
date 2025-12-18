/*
 * config.c
 *
 *  Created on: Dec 18, 2025
 *      Author: Hao Zhang
 */

#include "config.h"

// Default Values
#define DEFAULT_TOGGLE_FREQ 500
#define DEFAULT_PEDESTRIAN_DELAY 4000
#define DEFAULT_WALKING_DELAY 5000
#define DEFAULT_ORANGE_DELAY 2000
#define DEFAULT_GREEN_DELAY 5000
#define DEFAULT_RED_DELAY_MAX 10000

// Internal Storage
uint32_t g_toggleFreq = DEFAULT_TOGGLE_FREQ;
uint32_t g_pedestrianDelay = DEFAULT_PEDESTRIAN_DELAY;
uint32_t g_walkingDelay = DEFAULT_WALKING_DELAY;
uint32_t g_orangeDelay = DEFAULT_ORANGE_DELAY;
uint32_t g_greenDelay = DEFAULT_GREEN_DELAY;
uint32_t g_redDelayMax = DEFAULT_RED_DELAY_MAX;

void config_init(void) {
  // Reset to defaults if needed
  g_toggleFreq = DEFAULT_TOGGLE_FREQ;
  g_pedestrianDelay = DEFAULT_PEDESTRIAN_DELAY;
  g_walkingDelay = DEFAULT_WALKING_DELAY;
  g_orangeDelay = DEFAULT_ORANGE_DELAY;
  g_greenDelay = DEFAULT_GREEN_DELAY;
  g_redDelayMax = DEFAULT_RED_DELAY_MAX;
}

bool config_set_value(uint8_t id, uint16_t value) {
  switch (id) {
  case CONFIG_ID_TOGGLE_FREQ:
    // Rule: Must be > 0
    if (value > 0) {
      g_toggleFreq = value;
      return true;
    }
    break;

  case CONFIG_ID_PEDESTRIAN_DELAY:
    // Rule: Must be > orangeDelay (Implicit from Task 1 logic)
    if (value > g_orangeDelay) {
      g_pedestrianDelay = value;
      return true;
    }
    break;

  case CONFIG_ID_WALKING_DELAY:
    // Rule: Must be > 0
    if (value > 0) {
      g_walkingDelay = value;
      return true;
    }
    break;

  case CONFIG_ID_ORANGE_DELAY:
    // Rule: Must be > 0 AND checking pedestrianDelay constraint
    // If we increase orangeDelay, verify it doesn't violate pedestrianDelay >
    // orangeDelay
    if (value > 0 && g_pedestrianDelay > value) {
      g_orangeDelay = value;
      return true;
    }
    break;

  default:
    return false; // Unknown ID
  }
  return false; // Validation Failed
}
