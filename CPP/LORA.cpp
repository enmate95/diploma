/*
 * LORA.cpp
 *
 *  Created on: May 2, 2020
 *      Author: mate_pc
 */

#include "LORA.h"
#include "HCI.h"

LoRaDevice::LoRaDevice(UART_HandleTypeDef *_Handler) {
	Hci = new HCI(_Handler);
}

LoRaDevice::~LoRaDevice() {
	delete Hci;
}

int LoRaDevice::Send(uint8_t *data,uint16_t size) {
	// 1. check length
	if (size > (WIMOD_HCI_MSG_PAYLOAD_SIZE - 1)) {
	 // error
	 return -1;
	}
	// 2. init header
	Hci->TxMessage->SapID = LORAWAN_SAP_ID;
	Hci->TxMessage->MsgID = LORAWAN_MSG_SEND_UDATA_REQ;
	Hci->TxMessage->Length = 1 + size;
	// 3. init payload
	// 3.1 init port
	Hci->TxMessage->Payload[0] = Port;
	// 3.2 init radio message payload
	memcpy(&Hci->TxMessage->Payload[1], data, static_cast<unsigned int>(size));
	// 4. send Hci message with payload
	return Hci->SendMessage();
}

void LoRaDevice::Process() {
	Hci->Process();
}

int LoRaDevice::LoRaDevice::Connect() {
	Hci->TxMessage->SapID = LORAWAN_SAP_ID;
	Hci->TxMessage->MsgID = LORAWAN_MSG_JOIN_NETWORK_REQ;
	Hci->TxMessage->Length = 0;
	return Hci->SendMessage();
}

int LoRaDevice::Diconnect() {
	Hci->TxMessage->SapID = LORAWAN_SAP_ID;
	Hci->TxMessage->MsgID = LORAWAN_MSG_DEACTIVATE_DEVICE_REQ;
	Hci->TxMessage->Length = 0;
	// 2. send Hci message without payload
	return Hci->SendMessage();
}

int LoRaDevice::GetNtwkStatus() {
	Hci->TxMessage->SapID = LORAWAN_SAP_ID;
	Hci->TxMessage->MsgID = LORAWAN_MSG_GET_NWK_STATUS_REQ;
	Hci->TxMessage->Length = 0;
	// 2. send Hci message without payload
	return Hci->SendMessage();
}

int LoRaDevice::Reset() {
	Hci->TxMessage->SapID = DEVMGMT_SAP_ID;
	Hci->TxMessage->MsgID = DEVMGMT_MSG_RESET_REQ;
	Hci->TxMessage->Length = 0;
	// 2. send Hci message without payload
	return Hci->SendMessage();
}

LoRaDeviceStatus_t LoRaDevice::GetDeviceStatus() {
	return status;
}

int LoRaDevice::SetRadioConfig(RadioConfig_t RadioConfig) {
	Hci->TxMessage->SapID = LORAWAN_SAP_ID;
	Hci->TxMessage->MsgID = LORAWAN_MSG_SET_RSTACK_CONFIG_REQ;
	Hci->TxMessage->Length = 7;
	Hci->TxMessage->Payload[0] = RadioConfig.DataRateIndex;
	Hci->TxMessage->Payload[1] = RadioConfig.TXPwrLevel;
	Hci->TxMessage->Payload[2] = RadioConfig.Options;
	Hci->TxMessage->Payload[3] = RadioConfig.PowerSavingMode;
	Hci->TxMessage->Payload[4] = RadioConfig.NumOfRetrans;
	Hci->TxMessage->Payload[5] = RadioConfig.BandIndex;
	Hci->TxMessage->Payload[6] = RadioConfig.HeadarMACCmdCap;

	return Hci->SendMessage();
}

int LoRaDevice::GetRadioConfig() {
	Hci->TxMessage->SapID = LORAWAN_SAP_ID;
	Hci->TxMessage->MsgID = LORAWAN_MSG_GET_RSTACK_CONFIG_REQ;
	Hci->TxMessage->Length = 0;
	return Hci->SendMessage();
}

RadioConfig_t* LoRaDevice::GetCurrentRadioConfig() {
	return &RadioConfig;
}

int LoRaDevice::SetJoinParams(JoinParams_t _JoinParams) {
	Hci->TxMessage->SapID = LORAWAN_SAP_ID;
	Hci->TxMessage->MsgID = LORAWAN_MSG_SET_JOIN_PARAM_REQ;
	Hci->TxMessage->Length  = 24;
	memcpy(Hci->TxMessage->Payload,JoinParams.AppEUI,APP_EUI_SIZE);
	memcpy(Hci->TxMessage->Payload + 8,JoinParams.AppKey ,APP_KEY_SIZE);
	memcpy(JoinParams.AppEUI,_JoinParams.AppEUI,APP_EUI_SIZE);
	memcpy(JoinParams.AppKey,_JoinParams.AppKey,APP_EUI_SIZE);
	return Hci->SendMessage();
}

void LoRaDevice::Wakeup() {
	Hci->Wakeup();
}

TWiMOD_HCI_Message* HCICallback(TWiMOD_HCI_Message* rxMessage) {
	switch(rxMessage->SapID) {
		case DEVMGMT_SAP_ID:
			//Process_DeviceManagement_Message(rxMessage);
		break;

		case LORAWAN_SAP_ID:
			//Process_LoRaWAN_Message(rxMessage);
		break;
	}
	return rxMessage;
}


void  Process_LoRaWAN_Message(TWiMOD_HCI_Message* rxMessage) {

}

void  Process_DeviceManagement_Message(TWiMOD_HCI_Message* rxMessage) {

}


