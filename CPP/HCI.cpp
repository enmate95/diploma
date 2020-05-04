/*
 * HCI.cpp
 *
 *  Created on: May 2, 2020
 *      Author: mate_pc
 */

#include "HCI.h"

HCI::HCI(UART_HandleTypeDef *huart) {

	CRCErrors = 0;
	slip = new SLIP();
	crc = new CRC16();
	slip->SetRxBuffer(&RxMessage->SapID, sizeof(TWiMOD_HCI_Message) - sizeof(uint16_t));
	Handler = huart;
	RxSignal = 0;

	for(int i = 0; i < static_cast<int>(sizeof(RawRxBuffer)); i++)
		RawRxBuffer[i] = 0;
	HAL_UART_Receive_DMA(Handler,RawRxBuffer,sizeof(RawRxBuffer));
}

HCI::~HCI() {
	delete slip;
	delete crc;
}

int HCI::SendMessage() {

	 uint16_t crc16 = crc->Calc(&TxMessage->SapID, TxMessage->Length + WIMOD_HCI_MSG_HEADER_SIZE, CRC16_INIT_VALUE);
	 // 2.1 get 1's complement !!!
	 //
	 crc16 = ~crc16;
	 // 2.2 attach CRC16 and correct length, LSB first
	 //
	 TxMessage->Payload[TxMessage->Length] = LOBYTE(crc16);
	 TxMessage->Payload[TxMessage->Length + 1] = HIBYTE(crc16);
	 // 3. perform SLIP encoding
	 // - start transmission with SAP ID
	 // - correct length by header size
	 int txLength = slip->EncodeData(RawTxBuffer, sizeof(RawTxBuffer), &TxMessage->SapID, TxMessage->Length + WIMOD_HCI_MSG_HEADER_SIZE + WIMOD_HCI_MSG_FCS_SIZE);
	 // message ok ?
	 if (txLength > 0) {
		 // 4. send octet sequence over serial device
		 if (HAL_UART_Transmit(Handler,RawTxBuffer, txLength,100) == HAL_OK) {
		 // return ok
			 return 1;
		 }
	 }
	 // error - SLIP layer couldn't encode message - buffer to small ?
	 return -1;
}

void HCI::Wakeup() {
	uint8_t wakekup = 0xc0;
	for(int i = 0; i < 50; i++) {
		HAL_UART_Transmit(Handler,&wakekup,1,10);
	}
}

void HCI::ResetSignal() {
	RxSignal = 0;
}

int HCI::GetSignal() {
	return RxSignal;
}

void HCI::Process() {
	static uint32_t RawLength = 0;
	static int SlipLength;
	static uint8_t* SlipRxBuff;

	RawLength = (uint32_t)(50 - Handler->hdmarx->Instance->NDTR);
	if(RawLength > 0) {
		HAL_UART_DMAPause(Handler);
		for(int i = 0; i < static_cast<int>(RawLength); i++) {
			slip->DecodeData(RawRxBuffer[i]);
			if(slip->GetProcessSignal()) {
			SlipLength = slip->GetIndex();
			SlipRxBuff = slip->GetBuffer();
			if (slip->GetIndex() >= (WIMOD_HCI_MSG_HEADER_SIZE + WIMOD_HCI_MSG_FCS_SIZE)) {
					if (crc->Check(SlipRxBuff, SlipLength, CRC16_INIT_VALUE)) {
							//complete message info
							RxMessage->Length = SlipLength - (WIMOD_HCI_MSG_HEADER_SIZE + WIMOD_HCI_MSG_FCS_SIZE);
							RxSignal = 1;
					}
					else {
						CRCErrors++;
					}
				}
				slip->SetRxBuffer(&RxMessage->SapID, sizeof(TWiMOD_HCI_Message) - sizeof(uint16_t));
			}
		}
		for(int i = 0; i < 50; i++)
			RawRxBuffer[i] = 0;

		HAL_UART_DMAResume(Handler);
		HAL_UART_DMAStop(Handler);
		HAL_UART_Receive_DMA(Handler,RawRxBuffer,sizeof(RawRxBuffer));
	}
	else {
		HAL_UART_DMAResume(Handler);
	}
}

