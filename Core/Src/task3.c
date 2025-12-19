/**
  * @file task3.c
  * @brief Traffic Light State Machine implementation.
  * @date Dec 19, 2025
  * @author Hao Zhang
  */

#include "task3.h"
#include "task3.h"
#include "main.h"       // For HAL_Delay, HAL_GetTick
#include "cmsis_os.h"   // For osDelay
#include "input.h"      // For button reading
#include "config.h"     // For global configuration variables
// Assuming your new signal functions are declared here:
#include "led_driver.h"

// Input State
static bool input_v_ped_req = false;
static bool input_h_ped_req = false;
static uint32_t input_v_ped_req_time = 0;
static uint32_t input_h_ped_req_time = 0;
static bool input_v_car_busy = false;
static bool input_h_car_busy = false;

// --- Internal Logic Variables ---
static uint32_t v_arrival_time = 0;
static uint32_t h_arrival_time = 0;
static bool v_busy_prev = false;
static bool h_busy_prev = false;

static bool skip_yellow = false;


// --- State Definitions ---
typedef enum {
    STATE_INIT,
    STATE_VERTICAL_PASS,
    STATE_VERTICAL_STOP,
    STATE_HORIZONTAL_PASS,
    STATE_HORIZONTAL_STOP
} SystemState;



static SystemState current_state = STATE_INIT;
static uint32_t state_entry_time = 0;

// Configuration Variables are now in config.c/h
// (g_toggleFreq, g_pedestrianDelay, g_walkingDelay, g_orangeDelay, g_greenDelay, g_redDelayMax)


// Reads all inputs and updates state variables.
void task3_input_update(void) {
    // A. Latch Pedestrian Buttons
    if (input_read_pl1()) {
        if (!input_v_ped_req) {
            input_v_ped_req = true;
            input_v_ped_req_time = HAL_GetTick(); // Capture time
        }
        // Lamp handling moves to PedIndicatorTask
    }
    if (input_read_pl2()) {
        if (!input_h_ped_req) {
            input_h_ped_req = true;
             input_h_ped_req_time = HAL_GetTick(); // Capture time
        }
        // Lamp handling moves to PedIndicatorTask
    }

    // B. Update Car Sensors
    input_v_car_busy = input_read_tl1_car() || input_read_tl3_car();
    input_h_car_busy = input_read_tl2_car() || input_read_tl4_car();
    
    // C. Update Arrival Timers (Edge Detection)
    uint32_t now = HAL_GetTick();
    
    if ((input_v_ped_req || input_v_car_busy) && !v_busy_prev) {
        v_arrival_time = now;
    }
    v_busy_prev = input_v_car_busy;

    if ((input_h_ped_req || input_h_car_busy) && !h_busy_prev) {
        h_arrival_time = now;
    }
    h_busy_prev = input_h_car_busy;
}


// =========================================================
// 2. LOGIC HELPER FUNCTIONS
// =========================================================

// Handles the pedestrian crossing sequence.
static void service_pedestrian_sequence(bool is_vertical_ped) {
	TrafficFlowDirection flow = (is_vertical_ped) ? TRAFFIC_FLOW_VERTICAL : TRAFFIC_FLOW_HORIZONTAL;

    // 1. Turn off the "Wait" indicator (since they are now walking)
    set_lamp_pedestrian(flow, false);

    // 2. Turn on Green Walk Signal
    set_signal_pedestrian(flow, PED_LIGHT_GREEN);
    
    // 3. Blocking Delay (Freeze system for safety)
    osDelay(g_walkingDelay);

    // 4. Turn off Walk Signal (Back to Red)
    set_signal_pedestrian(flow, PED_LIGHT_RED);

    // 5. Clear the Input Latch
    if (is_vertical_ped) {
        input_v_ped_req = false;
    } else {
        input_h_ped_req = false;
    }
}


// Main Traffic Light State Machine.
void task3(void) {
    uint32_t current_time = HAL_GetTick();
    uint32_t time_in_state = current_time - state_entry_time;

    // Calculate wait times
    uint32_t h_wait_time = (input_h_car_busy) ? (current_time - h_arrival_time) : 0;
    uint32_t v_wait_time = (input_v_car_busy) ? (current_time - v_arrival_time) : 0;
    
    // Flag for "Ghost Town" or "Immediate Yield" logic

    switch (current_state) {
    
    // --- INITIALIZATION ---
    case STATE_INIT:
        // Set Safe Defaults
        set_signal(TRAFFIC_FLOW_VERTICAL, LIGHT_GREEN);
        set_signal(TRAFFIC_FLOW_HORIZONTAL, LIGHT_RED);
        set_signal_pedestrian(TRAFFIC_FLOW_VERTICAL, PED_LIGHT_RED);
        set_signal_pedestrian(TRAFFIC_FLOW_HORIZONTAL, PED_LIGHT_RED);
        
        // Ensure indicators are off
        set_lamp_pedestrian(TRAFFIC_FLOW_VERTICAL, false);
        set_lamp_pedestrian(TRAFFIC_FLOW_HORIZONTAL, false);

        current_state = STATE_VERTICAL_PASS;
        state_entry_time = current_time;

        skip_yellow = false;
        break;


    // --- VERTICAL FLOW (Green) ---
    case STATE_VERTICAL_PASS:
        // Ensure signals are correct
        set_signal(TRAFFIC_FLOW_VERTICAL, LIGHT_GREEN);
        set_signal(TRAFFIC_FLOW_HORIZONTAL, LIGHT_RED);


        // 1. Check & Service Horizontal Pedestrians (Opposite flow)
        if (input_h_ped_req) {
             service_pedestrian_sequence(false); // Service Horizontal
             // Note: Signal returns to Red automatically in helper
        }

        // 2. Check Switching Conditions
        bool switch_direction = false;



        // 2.4
        if (time_in_state > g_greenDelay) { // Use Config Variable
        	if(input_v_car_busy && !input_h_car_busy){
        		// 2.5
        		// Do nothing
        	} else {
        		switch_direction = true;
        	}
        }

        // 2.6
        if (input_v_car_busy && input_h_car_busy) {
        	if(h_wait_time > g_redDelayMax) { // Use Config Variable
        		switch_direction = true;
        	}
        }

        // 2.7
        if (!input_v_car_busy && input_h_car_busy) {
        	switch_direction = true;
        	skip_yellow = true;
        }

        if (input_v_ped_req) {
                 // Check deadline (R1.3)
                 uint32_t ped_wait_time = current_time - input_v_ped_req_time;
                 if (ped_wait_time > g_pedestrianDelay) {
                      switch_direction = true;
                 } else if (input_v_car_busy) {
					// If the opposite way is busy wait to max red.
					if(v_wait_time > g_redDelayMax) {
						switch_direction = true;
					}
				} else {
					// If the opposite way is not busy change direct.
					switch_direction = true;
				}
			}

        if (switch_direction) {
            current_state = STATE_VERTICAL_STOP;
            state_entry_time = current_time;
        }
        break;


    // --- VERTICAL STOP (Yellow) ---
    case STATE_VERTICAL_STOP:
        set_signal(TRAFFIC_FLOW_VERTICAL, LIGHT_YELLOW);
        set_signal(TRAFFIC_FLOW_HORIZONTAL, LIGHT_YELLOW);
        // Horizontal stays Red automatically (maintained from previous state)

        // 2.7 Skip the delay
        if (!skip_yellow) {
        	// Reset V arrival time if still busy (Wait time starts NOW)
        	if(input_v_car_busy) {
        		v_arrival_time = HAL_GetTick();
        	}
        	
        	// 2.3
            osDelay(g_orangeDelay); // Use Config
        }
        skip_yellow = false;

        current_state = STATE_HORIZONTAL_PASS;
        state_entry_time = HAL_GetTick();
        break;


    // --- HORIZONTAL FLOW (Green) ---
    case STATE_HORIZONTAL_PASS:
        set_signal(TRAFFIC_FLOW_HORIZONTAL, LIGHT_GREEN);
        set_signal(TRAFFIC_FLOW_VERTICAL, LIGHT_RED);


        // 1. Check & Service Vertical Pedestrians (Opposite flow)
        if (input_v_ped_req) {
             service_pedestrian_sequence(true); // Service Vertical
        }

        // 2. Check Switching Conditions


        // 2.4
        if (time_in_state > g_greenDelay) {
        	if(input_h_car_busy && !input_v_car_busy){
        		// 2.5
        		// Do nothing
        	} else {
        		switch_direction = true;
        	}


        }

        // 2.6
        if (input_h_car_busy && input_v_car_busy) {
        	if(v_wait_time > g_redDelayMax) {
        		switch_direction = true;
        	}
        }

        // 2.7
        if (!input_h_car_busy && input_v_car_busy) {
        	switch_direction = true;
        	skip_yellow = true;
        }

        // Make sure provide ability for pedestrian to walk if one way is always busy
        if (input_h_ped_req) {
        	if (input_h_car_busy) {
        		// If the opposite way is busy wait to max red.
        		if(h_wait_time > g_redDelayMax) {
					switch_direction = true;
				}
        	} else {
        		// If the opposite way is not busy change direct.
        		switch_direction = true;
        	}
        }

        if (switch_direction) {
            current_state = STATE_HORIZONTAL_STOP;
            state_entry_time = current_time;
        }
        break;


    // --- HORIZONTAL STOP (Yellow) ---
    case STATE_HORIZONTAL_STOP:
        set_signal(TRAFFIC_FLOW_VERTICAL, LIGHT_YELLOW);
        set_signal(TRAFFIC_FLOW_HORIZONTAL, LIGHT_YELLOW);
        // Vertical stays Red automatically

        if (!skip_yellow) {
            // Reset H arrival time if still busy
        	if(input_h_car_busy) {
        		h_arrival_time = HAL_GetTick();
        	}
        	
            osDelay(g_orangeDelay);
        }
        skip_yellow = false;

        current_state = STATE_VERTICAL_PASS;
        state_entry_time = HAL_GetTick();
        break;
    }
}

bool task3_is_vertical_ped_waiting(void) {
    return input_v_ped_req;
}

bool task3_is_horizontal_ped_waiting(void) {
    return input_h_ped_req;
}
