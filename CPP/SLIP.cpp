/*
 * SLIP.cpp
 *
 *  Created on: May 2, 2020
 *      Author: mate_pc
 */

#include "SLIP.h"

SLIP::SLIP() {
	// init decoder to idle state, no rx-buffer avaliable
	RxState = SLIPDEC_IDLE_STATE;
	RxIndex = 0;
	RxBuffer = 0;
	RxBufferSize = 0;
	// save message receiver callback
	// init encoder
	TxIndex = 0;
	TxBuffer = 0;
	TxBufferSize = 0;
	ProcessSignal = 0;
}

void SLIP::StoreTxByte(uint8_t txByte) {
	if (TxIndex < TxBufferSize)
		TxBuffer[TxIndex++] = txByte;
}

int SLIP::EncodeData(uint8_t* dstBuffer, int dstBufferSize, uint8_t* srcData, int srcLength) {
	// init TxBuffer
	TxBuffer = dstBuffer;
	// init TxIndex
	TxIndex = 0;
	// init size
	TxBufferSize = dstBufferSize;
	// send start of SLIP message
	StoreTxByte(SLIP_END);
	// iterate over all message bytes
	while(srcLength--){
		switch (*srcData) {
			case SLIP_END:
				StoreTxByte(SLIP_ESC);
				StoreTxByte(SLIP_ESC_END);
			break;

			case SLIP_ESC:
				StoreTxByte(SLIP_ESC);
				StoreTxByte(SLIP_ESC_ESC);
			break;

			default:
				StoreTxByte(*srcData);
			break;
		}
		// next byte
		srcData++;
	}
	// send end of SLIP message
	StoreTxByte(SLIP_END);
	// length ok ?
	if (TxIndex <= TxBufferSize)
		return TxIndex;
	// return tx length error
	return -1;
}

int SLIP::SetRxBuffer(uint8_t* rxBuffer, int rxBufferSize) {
	// receiver in IDLE state and client already registered ?
	if ((RxState == SLIPDEC_IDLE_STATE)) {
		// same buffer params
		RxBuffer = rxBuffer;
		RxBufferSize = rxBufferSize;
		// enable decoder
		RxState = SLIPDEC_START_STATE;
		return 1;
	}
	return 0;
}

void SLIP::StoresrcData(uint8_t srcData) {
	if (RxIndex < RxBufferSize)
		RxBuffer[RxIndex++] = srcData;
}

int SLIP::GetProcessSignal() {
	return ProcessSignal;
}

 void SLIP::ResetProcessSignal() {
	 ProcessSignal = 0;
 }

 uint8_t* SLIP::GetBuffer() {
	 return RxBuffer;
 }

 int SLIP::GetIndex() {
	 return RxIndex;
 }

void SLIP::DecodeData(uint8_t srcData) {
// iterate over all received bytes


	 // decode according to current state
	 switch(RxState) {
		 case SLIPDEC_START_STATE:
			 // start of SLIP frame ?
			 if(srcData == SLIP_END) {
				 // init read index
				 RxIndex = 0;
				 // next state
				 RxState = SLIPDEC_IN_FRAME_STATE;
			 }
		break;

		 case SLIPDEC_IN_FRAME_STATE:
		 switch(srcData) {
			 case SLIP_END:
			 // data received ?
			 if(RxIndex > 0) {
				ProcessSignal = 1;
				RxState = SLIPDEC_START_STATE;
			 }
			 // init read index
			 	 RxIndex = 0;
			 break;

			 case SLIP_ESC:
				 // enter escape sequence state
				 RxState = SLIPDEC_ESC_STATE;
				 break;
			 default:
				 // store byte
				 StoresrcData(srcData);
			 break;
		 }
		break;
		 case SLIPDEC_ESC_STATE:
		 switch(srcData) {
			 case SLIP_ESC_END:
				 StoresrcData(SLIP_END);
				// quit escape sequence state
				 RxState = SLIPDEC_IN_FRAME_STATE;
			 break;

			 case SLIP_ESC_ESC:
				 StoresrcData(SLIP_ESC);
				// quit escape sequence state
				 RxState = SLIPDEC_IN_FRAME_STATE;
			 break;

			 default:
				 // abort frame receiption
				 RxState = SLIPDEC_START_STATE;
			 break;
		 }
		break;
		 default:
		 break;
	 }
}
