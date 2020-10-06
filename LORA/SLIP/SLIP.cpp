/*
 * SLIP.cpp
 *
 *  Created on: May 2, 2020
 *      Author: mate_pc
 */

#include "SLIP.h"

SLIP::SLIP() {
	// init decoder to idle state, no rx-buffer avaliable
	RxState = SLIPDEC_START_STATE;
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
	TxBuffer = dstBuffer;
	TxIndex = 0;
	TxBufferSize = dstBufferSize;
	StoreTxByte(SLIP_END);
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
		srcData++;
	}
	StoreTxByte(SLIP_END);
	if (TxIndex <= TxBufferSize)
		return TxIndex;
	return -1;
}

int SLIP::SetRxBuffer(uint8_t* rxBuffer, int rxBufferSize) {
	if ((RxState == SLIPDEC_START_STATE)) {
		RxBuffer = rxBuffer;
		RxBufferSize = rxBufferSize;
		return 1;
	}
	return 0;
}

void SLIP::StoresrcData(uint8_t srcData) {
	if (RxIndex < RxBufferSize)
		RxBuffer[RxIndex++] = srcData;
}

 uint8_t* SLIP::GetBuffer() {
	 return RxBuffer;
 }

 int SLIP::GetIndex() {
	 return RxIndex;
 }

bool SLIP::DecodeData(uint8_t srcData) {

	 switch(RxState) {
		 case SLIPDEC_START_STATE:
			 if(srcData == SLIP_END) {
				 RxIndex = 0;
				 RxState = SLIPDEC_IN_FRAME_STATE;
			 }
		break;

		 case SLIPDEC_IN_FRAME_STATE:
		 switch(srcData) {
			 case SLIP_END:
			 if(RxIndex > 0) {
				RxState = SLIPDEC_START_STATE;
				return true;
			 }
			 break;

			 case SLIP_ESC:
				 RxState = SLIPDEC_ESC_STATE;
				 break;
			 default:
				 StoresrcData(srcData);
			 break;
		 }
		break;
		 case SLIPDEC_ESC_STATE:
		 switch(srcData) {
			 case SLIP_ESC_END:
				 StoresrcData(SLIP_END);
				 RxState = SLIPDEC_IN_FRAME_STATE;
			 break;

			 case SLIP_ESC_ESC:
				 StoresrcData(SLIP_ESC);
				 RxState = SLIPDEC_IN_FRAME_STATE;
			 break;

			 default:
				 RxState = SLIPDEC_START_STATE;
			 break;
		 }
		break;
		 default:
		 break;
	 }
		 return false;
}
