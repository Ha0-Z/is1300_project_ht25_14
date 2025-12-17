/*
 * LED_SPI.h
 *
 *  Created on: Dec 17, 2025
 *      Author: Daniel
 */

#ifndef SRC_LED_SPI_H_
#define SRC_LED_SPI_H_



#endif /* SRC_LED_SPI_H_ */
#include <stdint.h>

//uint8_t RED[3] = {0b1001,0b1001,0b1001};
//uint8_t YELLOW[3] = {0b10010,0b10,0b10};
//uint8_t GREEN[3] = {0b100100,0b10100,0b10100};
//Note: MSB first bit on bus. [0] = U3
static uint32_t TL1_Red 	= 	0b0001 << 16; 			//U1 Q0
static uint32_t TL1_Yellow = 	0b0010 << 16; 			//U1 Q1
static uint32_t TL1_Green 	= 	0b0100 << 16; 			//U1 Q2
static uint32_t PL1_Red 	= 0b1000 << 16; 			//U1 Q3s
static uint32_t PL1_Green 	= 0b0001 << 20; 	//U1 Q4
static uint32_t PL1_Blue 	= 0b0010 << 20; 	//U1 Q5

static uint32_t TL2_Red 	= 0b0001 <<  8; 	//U2 Q0
static uint32_t TL2_Yellow = 0b0010 <<  8; 	//U2 Q1
static uint32_t TL2_Green 	= 0b0100 <<  8; 	//U2 Q2
static uint32_t PL2_Red 	= 0b1000 <<  8; 	//U2 Q3
static uint32_t PL2_Green 	= 0b0001 << 12; 	//U2 Q4
static uint32_t PL2_Blue 	= 0b0010 << 12; 	//U2 Q5

static uint32_t TL3_Red 	= 0b0001 <<  0; 	//U3 Q0
static uint32_t TL3_Yellow = 0b0010 <<  0; 	//U3 Q1
static uint32_t TL3_Green 	= 0b0100 <<  0; 	//U3 Q2
static uint32_t TL4_Red 	= 0b1000 <<  0; 	//U3 Q3
static uint32_t TL4_Yellow = 0b0001 <<  4; 	//U3 Q4
static uint32_t TL4_Green 	= 0b0010 <<  4; 	//U3 Q5

//uint32_t led_buffer = 0b0001 << 20;
