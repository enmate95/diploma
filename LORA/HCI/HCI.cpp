/*
 * HCI.cpp
 *
 *  Created on: May 10, 2020
 *      Author: mate_pc
 */

#include "HCI.h"


LoRaHCI::HCI::HCI(UartDMA &serial,SLIP &slip,CRC16 & crc): serial(serial), slip(slip), crc(crc) {

	//give raw rx buffer address to slip encoder
	slip.SetRxBuffer(&RxMessage.SapID, sizeof(TWiMOD_HCI_Message) - sizeof(uint16_t));

	//init member variables
	CRCErrors = 0;
	RxMessage.CRC16[0] = 0;
	RxMessage.CRC16[1] = 0;
	RxMessage.Length = 0;
	RxMessage.MsgID = 0;
	RxMessage.SapID = 0;

	for(int j = 0; j < WIMOD_HCI_MSG_PAYLOAD_SIZE; j++) {
		RxMessage.Payload[j] = 0;
	}

	for(size_t i = 0; i < sizeof(RawRxBuffer.length); i++) {
		RawRxBuffer.data[i] = 0;
	}
}

bool LoRaHCI::HCI::SendMessage(TWiMOD_HCI_Message &TxMessage) {
	uint16_t crc16 = crc.Calc(&TxMessage.SapID, TxMessage.Length + WIMOD_HCI_MSG_HEADER_SIZE, CRC16_INIT_VALUE);
	crc16 = ~crc16;
	TxMessage.Payload[TxMessage.Length] = LOBYTE(crc16);
	TxMessage.Payload[TxMessage.Length + 1] = HIBYTE(crc16);

	int txLength = slip.EncodeData(RawTxBuffer, sizeof(RawTxBuffer), &TxMessage.SapID, TxMessage.Length + WIMOD_HCI_MSG_HEADER_SIZE + WIMOD_HCI_MSG_FCS_SIZE);

	if (txLength > 0) {
		if (serial.send(RawTxBuffer,static_cast<size_t>( txLength)) == HAL_OK) {
		 return true;
		}
	}
	return false;
}

void LoRaHCI::HCI::Wakeup() {
	uint8_t wakeup[40];
	for(int i = 0; i < 40; i++) {
		wakeup[i] = 0xc0;
	}

	serial.send(wakeup,sizeof(wakeup));
}

bool LoRaHCI::HCI::Process() {
	int SlipLength = 0;
	uint8_t* SlipRxBuff;
	serial.stopDMA();
	serial.copyBuffer(&RawRxBuffer);
	serial.startDMA();
	if(RawRxBuffer.length != 0) {
		for(uint32_t i = 0; i < RawRxBuffer.length; i++) {
			if(slip.DecodeData(RawRxBuffer.data[i])) {
				SlipLength = slip.GetIndex();
				SlipRxBuff = slip.GetBuffer();
				//if message bigger than maximum message size
				if (SlipLength >= (WIMOD_HCI_MSG_HEADER_SIZE + WIMOD_HCI_MSG_FCS_SIZE)) {
					//if CRC error happend
					if (!crc.Check(SlipRxBuff, SlipLength, CRC16_INIT_VALUE))
						CRCErrors++;
					RxMessage.Length = SlipLength - (WIMOD_HCI_MSG_HEADER_SIZE + WIMOD_HCI_MSG_FCS_SIZE);
					slip.SetRxBuffer(&RxMessage.SapID, sizeof(TWiMOD_HCI_Message) - sizeof(uint16_t));
					return true;
				}
				return false;
			}
		}
	}
	return false;
}

LoRaHCI::TWiMOD_HCI_Message* LoRaHCI::HCI::GetRxMessage() {
	return &RxMessage;
}

void LoRaHCI::HCI::Enable() {
	serial.startDMA();
}

void LoRaHCI::HCI::Disable() {
	serial.stopDMA();
}
