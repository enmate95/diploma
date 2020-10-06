/*
 * CRC16.h
 *
 *  Created on: May 4, 2020
 *      Author: mate_pc
 */

#ifndef CRC16_H_
#define CRC16_H_

#include <stdint.h>

#define CRC16_INIT_VALUE 0xFFFF 	// initial value for CRC algorithem
#define CRC16_GOOD_VALUE 0x0F47 	// constant compare value for check
#define CRC16_POLYNOM 0x8408 		// 16-BIT CRC CCITT POLYNOM

class CRC16 {
public:
	uint16_t Calc (uint8_t* data, uint16_t length, uint16_t initVal);
	int Check (uint8_t* data, uint16_t length, uint16_t initVal);
};



#endif /* CRC16_H_ */
