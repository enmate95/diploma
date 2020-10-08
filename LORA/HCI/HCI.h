/*
 * HCI2.h
 *
 *  Created on: May 10, 2020
 *      Author: mate_pc
 */

#ifndef HCI_H_
#define HCI_H_

#include "SLIP.h"
#include "CRC16.h"
#include "uartDevice.h"
#include <string.h>
#define WIMOD_HCI_MSG_HEADER_SIZE 2
#define WIMOD_HCI_MSG_PAYLOAD_SIZE 300
#define WIMOD_HCI_MSG_FCS_SIZE 2
#define LOBYTE(x) (x)
#define HIBYTE(x) ((uint16_t)(x) >> 8)
#define LORA_IT 0x01
#define RX_MESSAGE_SIZE 5

namespace LoRaHCI {

typedef struct {
	uint16_t Length;
	uint8_t SapID;
	uint8_t MsgID;
	uint8_t Payload[WIMOD_HCI_MSG_PAYLOAD_SIZE];
	uint8_t CRC16[WIMOD_HCI_MSG_FCS_SIZE];
}TWiMOD_HCI_Message;

class HCI {
public:
	HCI(UartDMA &serial,SLIP &slip,CRC16 & crc);
	~HCI() = default;
	bool SendMessage(TWiMOD_HCI_Message & TxMessage);
	void Wakeup();
	bool Process();
	TWiMOD_HCI_Message* GetRxMessage();
	void Enable();
	void Disable();
	UartDMA* GetSerial() { return &serial;}

private:
	UartDMA &serial;
	UartData_t RawRxBuffer;
	uint32_t CRCErrors;
	uint8_t RawTxBuffer[sizeof( TWiMOD_HCI_Message ) * 2 + 2];
	CRC16 &crc;
	SLIP &slip;
	TWiMOD_HCI_Message RxMessage;
};

}

#endif /* HCI2_H_ */
