/*
 * led_driver.h
 *
 *  Created on: Dec 17, 2025
 *      Author: Hao Zhang
 */
#ifndef INC_LED_DRIVER_H_
#define INC_LED_DRIVER_H_

#include <stdbool.h>
#include <stdint.h>

// Direction Enums
typedef enum { MODE_TL1 = 0, MODE_TL2, MODE_TL3, MODE_TL4 } TrafficDirection;

typedef enum { MODE_PL1 = 0, MODE_PL2 } PedestrianDirection;

// Light State Enums
typedef enum {
  LIGHT_OFF = 0,
  LIGHT_RED,
  LIGHT_YELLOW,
  LIGHT_GREEN,
  // Combined states if needed, though usually handled by individual calls or
  // specific logic
  LIGHT_RED_YELLOW
} TrafficLightState;

typedef enum {
  PED_LIGHT_OFF = 0,
  PED_LIGHT_RED,
  PED_LIGHT_GREEN
} PedestrianLightState;

// Driver Struct Interface
typedef struct {
  /**
  @brief Sets the state of a traffic signal (Car).
  @param dir The traffic light direction (e.g., MODE_TL1).
  @param state The desired light state (RED, YELLOW, GREEN).
  @return void
   */
  void (*set_traffic_signal)(TrafficDirection dir, TrafficLightState state);

  /**
  @brief Sets the state of a pedestrian signal.
  @param dir The pedestrian light direction (e.g., MODE_PL1).
  @param state The desired light state (RED, GREEN).
  @return void
   */
  void (*set_pedestrian_signal)(PedestrianDirection dir,
                                PedestrianLightState state);

  /**
  @brief Sets the state of the pedestrian indicator (Blue LED).
  @param dir The pedestrian direction.
  @param on True to turn on, False to turn off.
  @return void
   */
  void (*set_pedestrian_indicator)(PedestrianDirection dir, bool on);

  /**
  @brief Updates the physical LEDs by shifting out data via SPI.
  @param void
  @return void
   */
  void (*update_leds)(void);

  /**
  @brief Clears the internal buffer and turns off all LEDs.
  @param void
  @return void
   */
  void (*clear_all)(void);

  /**
  @brief DEBUG: Sets the raw 24-bit value of the shift registers.
  @param bits The 24-bit integer representing the LED states.
  @return void
   */
  void (*set_raw_bits)(uint32_t bits);

} TrafficLightDriver;

// Access the global driver instance
extern const TrafficLightDriver LED_Driver;

// High-Level Direction Enum
typedef enum {
    TRAFFIC_FLOW_VERTICAL,
    TRAFFIC_FLOW_HORIZONTAL
} TrafficFlowDirection;

// Direct Control Wrappers
void set_signal(TrafficFlowDirection dir, TrafficLightState color);
void set_signal_pedestrian(TrafficFlowDirection dir, PedestrianLightState color);
void set_lamp_pedestrian(TrafficFlowDirection dir, bool on);

/**
@brief Initializes the LED Driver and Shift Registers.
@param void
@return void
 */
void LED_Driver_Init(void);

#endif /* INC_LED_DRIVER_H_ */
