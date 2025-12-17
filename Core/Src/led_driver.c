/*
 * led_driver.c
 *
 *  Created on: Dec 17, 2025
 *      Author: Antigravity
 */

#include "led_driver.h"
#include "main.h"
#include "spi.h"

// Internal State Buffer
// We use a uint32_t to hold the state of all 24 bits
// mapping matches the bitmasks from the original code
// Byte 0 (LSB): U1 (TL1, PL1)
// Byte 1: U2 (TL2, PL2)
// Byte 2: U3 (TL3, TL4)
static uint32_t led_state_buffer = 0;

// Internal helper to set/clear bits
static void modify_bits(uint32_t mask, uint32_t value_bits) {
  led_state_buffer &= ~mask;      // Clear target bits
  led_state_buffer |= value_bits; // Set new bits
}

// Bit Definitions (Derived from original freertos.c)
#define U1_OFFSET 0
#define U2_OFFSET 8
#define U3_OFFSET 16

// Mappings for Traffic Lights (Red, Yellow, Green)
// Each TL uses 3 bits.
// TL1: U1 Q0-Q2
// TL2: U2 Q0-Q2
// TL3: U3 Q0-Q2
// TL4: U3 Q3-Q5

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
// PL1: U1 Q3-Q5 (Red, Green, Blue)
// PL2: U2 Q3-Q5 (Red, Green, Blue)

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
    break; // Q3
  case PED_LIGHT_GREEN:
    bits = 0b010 << shift;
    break; // Q4
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

  // Convert uint32 to 3 bytes (MSB first for this chain configuration)
  // U3 is last byte of uint32 (bits 16-23), but first byte sent to chain?
  // Based on freertos.c "[0] = U3", so txbuffer[0] should be bits 16-23.

  tx_buffer[0] = (led_state_buffer >> 16) & 0xFF; // U3
  tx_buffer[1] = (led_state_buffer >> 8) & 0xFF;  // U2
  tx_buffer[2] = (led_state_buffer >> 0) & 0xFF;  // U1

  // Transmit via SPI
  // Using hspi3 as seen in freertos.c
  HAL_SPI_Transmit(&hspi3, tx_buffer, 3, 10);

  // Latch the data (Pulse STCP)
  HAL_GPIO_WritePin(S595_STCP_GPIO_Port, S595_STCP_Pin, GPIO_PIN_SET);
  // Short delay might be needed, but HAL_GPIO includes some overhead usually
  // sufficient. If strict timing needed: for(int i=0;i<10;i++) __NOP();
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
  // Ensure Reset pin is high (Active Low reset usually, or check logic)
  // freertos.c:
  // HAL_GPIO_WritePin(S595_Reset_GPIO_Port,S595_Reset_Pin,GPIO_PIN_SET);
  HAL_GPIO_WritePin(S595_Reset_GPIO_Port, S595_Reset_Pin, GPIO_PIN_SET);

  // Clear initial state
  clear_all_impl();
}
