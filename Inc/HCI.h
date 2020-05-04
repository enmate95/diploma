/*
 * HCI.h
 *
 *  Created on: May 2, 2020
 *      Author: mate_pc
 */

#ifndef HCI_H_
#define HCI_H_

#include "SLIP.h"
#include "CRC16.h"
#include "stm32f4xx_hal.h"

#define WIMOD_HCI_MSG_HEADER_SIZE 2
#define WIMOD_HCI_MSG_PAYLOAD_SIZE 300
#define WIMOD_HCI_MSG_FCS_SIZE 2
#define LOBYTE(x) (x)
#define HIBYTE(x) ((uint16_t)(x) >> 8)

typedef struct {
	// Payload Length Information,
	// this field not transmitted over UART interface !!!
	uint16_t Length;
	// Service Access Point Identifier
	uint8_t SapID;
	// Message Identifier
	uint8_t MsgID;
	// Payload Field
	uint8_t Payload[WIMOD_HCI_MSG_PAYLOAD_SIZE];
	// Frame Check Sequence Field
	uint8_t CRC16[WIMOD_HCI_MSG_FCS_SIZE];
}TWiMOD_HCI_Message;


class HCI {
public:
	HCI(UART_HandleTypeDef *huart);
	~HCI();
	// Send HCI Message
	int SendMessage();
	// Receiver Process
	void Wakeup();
	void Process();
	void ResetSignal();
	int GetSignal();
	TWiMOD_HCI_Message* TxMessage;

private:
	UART_HandleTypeDef *Handler;
	// CRC Error counter
	uint32_t CRCErrors;
	// RxMessage
	TWiMOD_HCI_Message* RxMessage;
	uint8_t RawTxBuffer[sizeof( TWiMOD_HCI_Message ) * 2 + 2];
	uint8_t RawRxBuffer[100];
	// Receiver callback
	CRC16 *crc;
	SLIP *slip;
	int RxSignal;
};


#endif /* HCI_H_ */
