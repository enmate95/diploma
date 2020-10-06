/*
 * SLIP.h
 *
 *  Created on: May 2, 2020
 *      Author: mate_pc
 */

#ifndef SLIP_H_
#define SLIP_H_

#include <stdint.h>

// SLIP Protocol Characters
#define SLIP_END 0xC0
#define SLIP_ESC 0xDB
#define SLIP_ESC_END 0xDC
#define SLIP_ESC_ESC 0xDD
// SLIP Receiver/Decoder States
#define SLIPDEC_IDLE_STATE 0
#define SLIPDEC_START_STATE 1
#define SLIPDEC_IN_FRAME_STATE 2
#define SLIPDEC_ESC_STATE 3

 class SLIP {
 public:
	 SLIP();
	 int  SetRxBuffer(uint8_t* rxBuffer, int rxBufferSize);
	 int EncodeData(uint8_t* dstBuffer, int txBufferSize, uint8_t* srcData,int srcLength);
	 bool DecodeData(uint8_t srcData);
	 uint8_t* GetBuffer();
	 int GetIndex();

 private:
	void StoresrcData(uint8_t srcData);
	void StoreTxByte(uint8_t txByte);
	int RxState;
	int RxIndex;
	int RxBufferSize;
	uint8_t* RxBuffer;
	int TxIndex;
	int TxBufferSize;
	uint8_t* TxBuffer;
	int ProcessSignal;
 };


#endif /* SLIP_H_ */
