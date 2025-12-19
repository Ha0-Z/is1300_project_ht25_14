#include "led_driver.h"
#include "main.h"
#include "spi.h"

// Internal State Buffer (24 bits)
static uint32_t led_state_buffer = 0;

// Modify bits in the buffer
static void modify_bits(uint32_t mask, uint32_t value_bits) {
  led_state_buffer &= ~mask;      // Clear target bits
  led_state_buffer |= value_bits; // Set new bits
}

// Bit Definitions (Derived from original freertos.c)
#define U1_OFFSET 0
#define U2_OFFSET 8
#define U3_OFFSET 16

// Mappings for Traffic Lights (Red, Yellow, Green)

// Helper to get shift amount for a given direction
static int get_tl_shift(TrafficDirection dir) {
  switch (dir) {
  case MODE_TL1:
    return U1_OFFSET + 0;
  case MODE_TL2:
    return U2_OFFSET + 0;
  case MODE_TL3:
    return U3_OFFSET + 0;
  case MODE_TL4:
    return U3_OFFSET + 3;
  default:
    return 0;
  }
}

static void set_traffic_signal_impl(TrafficDirection dir,
                                    TrafficLightState state) {
  int shift = get_tl_shift(dir);
  uint32_t mask = 0b111 << shift; // 3 bits for R, Y, G
  uint32_t bits = 0;

  switch (state) {
  case LIGHT_RED:
    bits = 0b001 << shift;
    break;
  case LIGHT_YELLOW:
    bits = 0b010 << shift;
    break;
  case LIGHT_GREEN:
    bits = 0b100 << shift;
    break;
  case LIGHT_RED_YELLOW:
    bits = 0b011 << shift;
    break; // Optional
  case LIGHT_OFF:
  default:
    bits = 0;
    break;
  }

  // Modify the global state
  modify_bits(mask, bits);
}

// Mappings for Pedestrian Lights

static int get_pl_shift(PedestrianDirection dir) {
  switch (dir) {
  case MODE_PL1:
    return U1_OFFSET + 3;
  case MODE_PL2:
    return U2_OFFSET + 3;
  default:
    return 0;
  }
}

static void set_pedestrian_signal_impl(PedestrianDirection dir,
                                       PedestrianLightState state) {
  int shift = get_pl_shift(dir);
  uint32_t mask = 0b111 << shift;
  uint32_t bits = 0;

  switch (state) {
  case PED_LIGHT_RED:
    bits = 0b001 << shift;
    break;
  case PED_LIGHT_GREEN:
    bits = 0b010 << shift;
    break;
  case PED_LIGHT_OFF:
  default:
    bits = 0;
    break;
  }

  modify_bits(mask, bits);
}

static void set_pedestrian_indicator_impl(PedestrianDirection dir, bool on) {
  int shift = get_pl_shift(dir);
  // Blue LED is Q5, which is bit 2 relative to shift (Q3=0, Q4=1, Q5=2)
  uint32_t mask = 0b100 << shift;
  uint32_t bits = on ? (0b100 << shift) : 0;

  modify_bits(mask, bits);
}

static void update_leds_impl(void) {
  uint8_t tx_buffer[3];

  // Convert uint32 to 3 bytes (MSB first)

  tx_buffer[0] = (led_state_buffer >> 16) & 0xFF; // U3
  tx_buffer[1] = (led_state_buffer >> 8) & 0xFF;  // U2
  tx_buffer[2] = (led_state_buffer >> 0) & 0xFF;  // U1

  // Transmit via SPI.
  // Using hspi3 as seen in freertos.c.
  HAL_SPI_Transmit(&hspi3, tx_buffer, 3, 10);

  // Latch data.
  HAL_GPIO_WritePin(S595_STCP_GPIO_Port, S595_STCP_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(S595_STCP_GPIO_Port, S595_STCP_Pin, GPIO_PIN_RESET);
}

static void clear_all_impl(void) {
  led_state_buffer = 0;
  update_leds_impl();
}

static void set_raw_bits_impl(uint32_t bits) {
  led_state_buffer = bits;
  update_leds_impl();
}

// Global Instance
const TrafficLightDriver LED_Driver = {
    .set_traffic_signal = set_traffic_signal_impl,
    .set_pedestrian_signal = set_pedestrian_signal_impl,
    .set_pedestrian_indicator = set_pedestrian_indicator_impl,
    .update_leds = update_leds_impl,
    .clear_all = clear_all_impl,
    .set_raw_bits = set_raw_bits_impl};

void LED_Driver_Init(void) {
  // Ensure Reset pin is high.
  HAL_GPIO_WritePin(S595_Reset_GPIO_Port, S595_Reset_Pin, GPIO_PIN_SET);

  // Clear initial state
  clear_all_impl();
}

// Direct Control Wrappers
void set_signal(TrafficFlowDirection dir, TrafficLightState color) {
  if (dir == TRAFFIC_FLOW_VERTICAL) {
      LED_Driver.set_traffic_signal(MODE_TL1, color);
      LED_Driver.set_traffic_signal(MODE_TL3, color);
  } else {
      LED_Driver.set_traffic_signal(MODE_TL2, color);
      LED_Driver.set_traffic_signal(MODE_TL4, color);
  }
  LED_Driver.update_leds();
}

void set_signal_pedestrian(TrafficFlowDirection dir, PedestrianLightState color) {
  if (dir == TRAFFIC_FLOW_VERTICAL) {
      LED_Driver.set_pedestrian_signal(MODE_PL1, color);
  } else {
      LED_Driver.set_pedestrian_signal(MODE_PL2, color);
  }
  LED_Driver.update_leds();
}

void set_lamp_pedestrian(TrafficFlowDirection dir, bool on) {
  if (dir == TRAFFIC_FLOW_VERTICAL) {
      LED_Driver.set_pedestrian_indicator(MODE_PL1, on);
  } else {
      LED_Driver.set_pedestrian_indicator(MODE_PL2, on);
  }
  LED_Driver.update_leds();
}
