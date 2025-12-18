/*
 * config.h
 *
 *  Created on: Dec 18, 2025
 *      Author: Hao Zhang
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

#include <stdbool.h>
#include <stdint.h>

// Global Configuration Variables
extern uint32_t g_toggleFreq;
extern uint32_t g_pedestrianDelay;
extern uint32_t g_walkingDelay;
extern uint32_t g_orangeDelay;
extern uint32_t g_greenDelay;
extern uint32_t g_redDelayMax;

// Configuration IDs
#define CONFIG_ID_TOGGLE_FREQ 0x01
#define CONFIG_ID_PEDESTRIAN_DELAY 0x02
#define CONFIG_ID_WALKING_DELAY 0x03
#define CONFIG_ID_ORANGE_DELAY 0x04

// Function to set value with validation
// Returns true if accepted, false if rejected.
bool config_set_value(uint8_t id, uint16_t value);

void config_init(void);

#endif /* INC_CONFIG_H_ */
