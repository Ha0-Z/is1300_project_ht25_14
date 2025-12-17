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
  // Sets the state of a traffic signal (Car)
  void (*set_traffic_signal)(TrafficDirection dir, TrafficLightState state);

  // Sets the state of a pedestrian signal
  void (*set_pedestrian_signal)(PedestrianDirection dir,
                                PedestrianLightState state);

  // Sets the state of the pedestrian indicator (Blue LED / "Walking Person")
  void (*set_pedestrian_indicator)(PedestrianDirection dir, bool on);

  // Updates the physical LEDs (shifts data out)
  void (*update_leds)(void);

  // Clears the internal buffer (all off)
  void (*clear_all)(void);

  // DEBUG: Set raw 24-bit value
  void (*set_raw_bits)(uint32_t bits);

} TrafficLightDriver;

// Access the global driver instance
extern const TrafficLightDriver LED_Driver;

// Initialization function (call this in main or before task starts)
void LED_Driver_Init(void);

#endif /* INC_LED_DRIVER_H_ */
