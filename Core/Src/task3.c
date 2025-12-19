/*
 * task3.c
 *
 * Integrated State Machine with Signal Controller API
 * Date: Dec 19, 2025
 */

#include "task3.h"
#include "main.h"       // For HAL_Delay, HAL_GetTick
#include "input.h"      // For button reading
// Assuming your new signal functions are declared here:
#include "led_driver.h"

// --- Shared Data (Input Interface) ---
static bool input_v_ped_req = false;   // Latched: True until serviced
static bool input_h_ped_req = false;   // Latched: True until serviced
static bool input_v_car_busy = false;  // Instantaneous status
static bool input_h_car_busy = false;  // Instantaneous status

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

// Configuration Constants
#define ORANGE_DELAY_MS   500
#define WALKING_DELAY_MS  3000
#define MIN_GREEN_MS      10000
#define MAX_RED_WAIT_MS   5000


// =========================================================
// 1. INPUT COLLECTION (Run 100x more often)
// =========================================================
void collectInput(void) {
    // A. Latch Pedestrian Buttons
    if (input_read_pl1()) {
        input_v_ped_req = true;
        // Optional: Turn on "Wait" indicator lamp immediately
        set_lamp_pedestrian(TRAFFIC_FLOW_VERTICAL, true); 
    }
    if (input_read_pl2()) {
        input_h_ped_req = true;
        // Optional: Turn on "Wait" indicator lamp immediately
        set_lamp_pedestrian(TRAFFIC_FLOW_HORIZONTAL, true);
    }

    // B. Update Car Sensors
    input_v_car_busy = input_read_tl1_car() || input_read_tl3_car();
    input_h_car_busy = input_read_tl2_car() || input_read_tl4_car();
    
    // C. Update Arrival Timers (Edge Detection)
    uint32_t now = HAL_GetTick();
    
    if (input_v_car_busy && !v_busy_prev) {
        v_arrival_time = now;
    }
    v_busy_prev = input_v_car_busy;

    if (input_h_car_busy && !h_busy_prev) {
        h_arrival_time = now;
    }
    h_busy_prev = input_h_car_busy;
}


// =========================================================
// 2. LOGIC HELPER FUNCTIONS
// =========================================================

/*
 * Blocking sequence to let pedestrians cross.
 * Pauses the traffic logic to ensure safety.
 */
static void service_pedestrian_sequence(bool is_vertical_ped) {
	TrafficFlowDirection flow = (is_vertical_ped) ? TRAFFIC_FLOW_VERTICAL : TRAFFIC_FLOW_HORIZONTAL;

    // 1. Turn off the "Wait" indicator (since they are now walking)
    set_lamp_pedestrian(flow, false);

    // 2. Turn on Green Walk Signal
    set_signal_pedestrian(flow, PED_LIGHT_GREEN);
    
    // 3. Blocking Delay (Freeze system for safety)
    HAL_Delay(WALKING_DELAY_MS);

    // 4. Turn off Walk Signal (Back to Red)
    set_signal_pedestrian(flow, PED_LIGHT_RED);

    // 5. Clear the Input Latch
    if (is_vertical_ped) {
        input_v_ped_req = false;
    } else {
        input_h_ped_req = false;
    }
}


// =========================================================
// 3. MAIN TASK (Run 1x per cycle)
// =========================================================
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

        v_arrival_time = HAL_GetTick();

        // 1. Check & Service Horizontal Pedestrians (Opposite flow)
        if (input_h_ped_req) {
             service_pedestrian_sequence(false); // Service Horizontal
             // Note: Signal returns to Red automatically in helper
        }

        // 2. Check Switching Conditions
        bool switch_v = false;



        // 2.4
        if (time_in_state > MIN_GREEN_MS) {
        	if(input_v_car_busy && !input_h_car_busy){
        		// 2.5
        		// Do nothing
        	} else {
        		switch_v = true;
        	}
        }

        // 2.6
        if (input_v_car_busy && input_h_car_busy) {
        	if(h_wait_time > MAX_RED_WAIT_MS) {
        		switch_v = true;
        	}
        }

        // 2.7
        if (!input_v_car_busy && input_h_car_busy) {
        	switch_v = true;
        	skip_yellow = true;
        }

//        // Condition A: Gap Logic (Vertical Empty + Others Waiting)
//        if (!input_v_car_busy && (input_h_car_busy || time_in_state > MIN_GREEN_MS)) {
//            switch_v = true;
//            // Optional: If V is empty and H is waiting, we could set skip_yellow = true;
//        }
//        // Condition B: Fairness / Max Wait
//        else if (h_wait_time > MAX_RED_WAIT_MS) {
//            switch_v = true;
//        }
//
        if (switch_v) {
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
        	// 2.3
            HAL_Delay(ORANGE_DELAY_MS);
        }
        skip_yellow = false;

        current_state = STATE_HORIZONTAL_PASS;
        state_entry_time = HAL_GetTick();
        break;


    // --- HORIZONTAL FLOW (Green) ---
    case STATE_HORIZONTAL_PASS:
        set_signal(TRAFFIC_FLOW_HORIZONTAL, LIGHT_GREEN);
        set_signal(TRAFFIC_FLOW_VERTICAL, LIGHT_RED);

        h_arrival_time = HAL_GetTick();


        // 1. Check & Service Vertical Pedestrians (Opposite flow)
        if (input_v_ped_req) {
             service_pedestrian_sequence(true); // Service Vertical
        }

        // 2. Check Switching Conditions

        bool switch_h = false;


        // 2.4
        if (time_in_state > MIN_GREEN_MS) {
        	if(input_h_car_busy && !input_v_car_busy){
        		// 2.5
        		// Do nothing
        	} else {
        		switch_v = true;
        	}


        }

        // 2.6
        if (input_h_car_busy && input_v_car_busy) {
        	if(v_wait_time > MAX_RED_WAIT_MS) {
        		switch_v = true;
        	}
        }

        // 2.7
        if (!input_h_car_busy && input_v_car_busy) {
        	switch_v = true;
        	skip_yellow = true;
        }

        // Make sure provide ability for pedestrian to walk if one way is always busy


//        // Condition A: Gap Logic
//        if (!input_h_car_busy && (input_v_car_busy || time_in_state > MIN_GREEN_MS)) {
//            switch_h = true;
//        }
//        // Condition B: Fairness
//        else if (v_wait_time > MAX_RED_WAIT_MS) {
//            switch_h = true;
//        }
//
        if (switch_v) {
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
            HAL_Delay(ORANGE_DELAY_MS);
        }
        skip_yellow = false;

        current_state = STATE_VERTICAL_PASS;
        state_entry_time = HAL_GetTick();
        break;
    }
}
