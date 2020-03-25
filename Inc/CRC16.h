/*
 * CRC16.h
 *
 *  Created on: Mar 25, 2020
 *      Author: mate_pc
 */

/*
 Copyright (c) 2009 IMST GmbH.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are NOT permitted without prior written permission
 of the IMST GmbH.

 THIS SOFTWARE IS PROVIDED BY THE IMST GMBH AND CONTRIBUTORS ``AS IS'' AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE IMST GMBH OR CONTRIBUTORS BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 THIS SOFTWARE IS CLASSIFIED AS: CONFIDENTIAL

 *******************************************************************************/
#ifndef CRC16_H_
#define CRC16_H_

#include "stm32f4xx_hal.h"

#define CRC16_INIT_VALUE    0xFFFF    //!< initial value for CRC algorithem
#define CRC16_GOOD_VALUE    0x0F47    //!< constant compare value for check
#define CRC16_POLYNOM       0x8408    //!< 16-BIT CRC CCITT POLYNOM

uint16_t CRC16_Calc (uint8_t* data, uint16_t length, uint16_t initVal);
bool CRC16_Check(uint8_t* data, uint16_t length, uint16_t initVal);

#endif /* CRC16_H_ */
