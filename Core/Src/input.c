/*
 * input.c
 *
 *  Created on: Dec 18, 2025
 *      Author: Hao Zhang
 */

#include "input.h"
#include "main.h"

// Helper to read a pin (Active Low: Pressed = 0)
static bool is_pressed(GPIO_TypeDef *port, uint16_t pin) {
  return (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_RESET);
}

JoystickState input_read_joystick(void) {
  JoystickState state = {0};

  // Joystick Up (A) - PC8
  state.up = is_pressed(button3_A_GPIO_Port, button3_A_Pin);

  // Joystick Down (B) - PC6
  state.down = is_pressed(button3_B_GPIO_Port, button3_B_Pin);

  // Joystick Left (C) - PC5
  state.left = is_pressed(button3_C_GPIO_Port, button3_C_Pin);

  // Joystick Right (D) - PB9
  state.right = is_pressed(button3_D_GPIO_Port, button3_D_Pin);

  // Joystick Center - PB8
  state.center = is_pressed(button3_center_GPIO_Port, button3_center_Pin);

  return state;
}

bool input_read_pl1(void) {
  return is_pressed(PL1_Switch_GPIO_Port, PL1_Switch_Pin);
}

bool input_read_pl2(void) {
  return is_pressed(PL2_Switch_GPIO_Port, PL2_Switch_Pin);
}

// Car Switches (TL1-TL4)
// Connected to Ground when pressed. Internal Pull-Ups are enabled in gpio.c.
// Logic: LOW (GPIO_PIN_RESET) = Pressed (true)
bool input_read_tl1_car(void) {
	if( HAL_GPIO_ReadPin(TL1_Car_GPIO_Port, TL1_Car_Pin) == GPIO_PIN_SET ){
		return true;
	}else {
	    return false;
	}
}

bool input_read_tl2_car(void) {
  if (HAL_GPIO_ReadPin(TL2_Car_GPIO_Port, TL2_Car_Pin) == GPIO_PIN_SET) {
    return true;
  } else {
    return false;
  }
}

bool input_read_tl3_car(void) {
  if (HAL_GPIO_ReadPin(TL3_Car_GPIO_Port, TL3_Car_Pin) == GPIO_PIN_SET) {
    return true;
  } else {
    return false;
  }
}

bool input_read_tl4_car(void) {
  // TL4 is on PA10
  if (HAL_GPIO_ReadPin(TL4_Car_GPIO_Port, TL4_Car_Pin) == GPIO_PIN_SET) {
    return true;
  } else {
    return false;
  }
}
