/*
 * task5.c
 *
 *  Created on: Dec 18, 2025
 *      Author: Hao Zhang
 */

#include "task5.h"
#include "config.h"
#include "usart.h"
#include <stdbool.h>
#include <stdio.h> // for snprintf
#include <string.h> // for strlen

// Ring Buffer Variables
#define RING_BUF_SIZE 64
static uint8_t rx_byte; // Temp buffer for HAL
static uint8_t ring_buffer[RING_BUF_SIZE];
static volatile int head = 0; // Write Index
static int tail = 0;          // Read Index

// Parser State (Static)
static uint8_t parse_buffer[4];
static int parse_index = 0;

void task5_uart_init(void) {
    // Start listening for 1 byte
    HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        // Push to Ring Buffer
        int next_head = (head + 1) % RING_BUF_SIZE;
        if (next_head != tail) {
            ring_buffer[head] = rx_byte;
            head = next_head;
        }
        // Restart listening immediately
        HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
    }
}

void task5_poller(void) {
    // 1. Calculate available data in Ring Buffer
    // Head and Tail are indices 0..63
    int available = (head - tail + RING_BUF_SIZE) % RING_BUF_SIZE;

    // 2. Process ALL full packets available
    while (available >= 4) {
        // Peek at the 4 bytes (Handle wrapping manually for each byte)
        uint8_t b0 = ring_buffer[(tail + 0) % RING_BUF_SIZE];
        uint8_t b1 = ring_buffer[(tail + 1) % RING_BUF_SIZE];
        uint8_t b2 = ring_buffer[(tail + 2) % RING_BUF_SIZE];
        uint8_t b3 = ring_buffer[(tail + 3) % RING_BUF_SIZE];

        // 3. Check Sync (Byte 1 must be 0x00)
        if (b1 == 0x00) {
            // --- VALID PACKET ---
            uint16_t val = (b2 << 8) | b3;
            bool success = config_set_value(b0, val);

            // Send ACK
            uint8_t ack = success ? 0x01 : 0x00;
            HAL_UART_Transmit(&huart2, &ack, 1, 100);

            // Feedback
            if (success) {
               char msg[64];
               snprintf(msg, sizeof(msg), "\r\nConfig %d Set to %d\r\n", b0, val);
               HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), 100);
            }

            // Consume Packet (Move Tail by 4)
            tail = (tail + 4) % RING_BUF_SIZE;
            available -= 4;

        } else {
            // --- INVALID SYNC ---
            // The Packet is misaligned.
            // Move Tail by 1 to "slide" the window and try finding 0x00 again.
            tail = (tail + 1) % RING_BUF_SIZE;
            available -= 1;
        }
    }
}
