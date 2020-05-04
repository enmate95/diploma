/*
 * LORA.h
 *
 *  Created on: May 1, 2020
 *      Author: mate_pc
 */

#ifndef LORA_H_
#define LORA_H_

#include "HCI.h"
#include "string.h"

#define DEVMGMT_SAP_ID 0x01
#define LORAWAN_SAP_ID 0x10

#define DEVMGMT_MSG_PING_REQ 0x01
#define DEVMGMT_MSG_PING_RSP 0x02

#define DEVMGMT_MSG_GET_DEVICE_INFO_REQ 0x03
#define DEVMGMT_MSG_GET_DEVICE_INFO_RSP 0x04

#define DEVMGMT_MSG_RESET_REQ 0x07
#define DEVMGMT_MSG_RESET_RSP 0x08

#define DEVMGMT_MSG_SET_OPMODE_REQ 0x09
#define DEVMGMT_MSG_SET_OPMODE_RSP 0x0A
#define DEVMGMT_MSG_GET_OPMODE_REQ 0x0B
#define DEVMGMT_MSG_GET_OPMODE_RSP 0x0C

#define DEVMGMT_MSG_GET_DEVICE_STATUS_REQ 0x17
#define DEVMGMT_MSG_GET_DEVICE_STATUS_RSP 0x18

#define DEVMGMT_MSG_POWER_UP_IND 0x20
#define DEVMGMT_MSG_GET_HCI_CFG_REQ 0x43
#define DEVMGMT_MSG_GET_HCI_CFG_RSP 0x44
#define DEVMGMT_MSG_SET_HCI_CFG_REQ 0x41
#define DEVMGMT_MSG_SET_HCI_CFG_RSP 0x42

#define DEVMGMT_STATUS_OK 0x00 						//Operation successful
#define DEVMGMT_STATUS_ERROR 0x01 					//Operation failed
#define DEVMGMT_STATUS_CMD_NOT_SUPPORTED 0x02 		//Command is not supported
#define DEVMGMT_STATUS_WRONG_PARAMETER 0x03			//HCI message contains wrong parameter


#define LORAWAN_MSG_SEND_UDATA_REQ 0x0D
#define LORAWAN_MSG_SEND_UDATA_RSP 0x0E
#define LORAWAN_MSG_SEND_UDATA_IND 0x0F
#define LORAWAN_MSG_RECV_UDATA_IND 0x10
#define LORAWAN_MSG_SEND_CDATA_REQ 0x11
#define LORAWAN_MSG_SEND_CDATA_RSP 0x12
#define LORAWAN_MSG_SEND_CDATA_IND 0x13
#define LORAWAN_MSG_RECV_CDATA_IND 0x14
#define LORAWAN_MSG_RECV_ACK_IND 0x15
#define LORAWAN_MSG_RECV_NODATA_IND 0x16

#define LORAWAN_MSG_ACTIVATE_DEVICE_REQ 0x01
#define LORAWAN_MSG_ACTIVATE_DEVICE_RSP 0x02
#define LORAWAN_MSG_DEACTIVATE_DEVICE_REQ 0x21
#define LORAWAN_MSG_DEACTIVATE_DEVICE_RSP 0x22
#define LORAWAN_MSG_GET_NWK_STATUS_REQ 0x29
#define LORAWAN_MSG_GET_NWK_STATUS_RSP 0x2A


#define LORAWAN_MSG_SET_JOIN_PARAM_REQ 0x05
#define LORAWAN_MSG_SET_JOIN_PARAM_RSP 0x06
#define LORAWAN_MSG_JOIN_NETWORK_REQ 0x09
#define LORAWAN_MSG_JOIN_NETWORK_RSP 0x0A
#define LORAWAN_MSG_JOIN_NETWORK_TX_IND 0x0B
#define LORAWAN_MSG_JOIN_NETWORK_IND 0x0C

#define LORAWAN_MSG_SET_RSTACK_CONFIG_REQ 0x19
#define LORAWAN_MSG_SET_RSTACK_CONFIG_RSP 0x1A
#define LORAWAN_MSG_GET_RSTACK_CONFIG_REQ 0x1B
#define LORAWAN_MSG_GET_RSTACK_CONFIG_RSP 0x1C

#define LORAWAN_STATUS_OK 0x00 						//Operation successful
#define LORAWAN_STATUS_ERROR 0x01 					//Operation failed
#define LORAWAN_STATUS_CMD_NOT_SUPPORTED 0x02 		//Command is not supported
#define LORAWAN_STATUS_WRONG_PARAMETER 0x03 		//HCI message contains wrong parameter
#define LORAWAN_STATUS_WRONG_DEVICE_MODE 0x04 		//Stack is running in a wrong mode
#define LORAWAN_STATUS_DEVICE_NOT_ACTIVATED 0x05 	//Device is not activated
#define LORAWAN_STATUS_DEVICE_BUSY 0x06 			//Device is busy, command rejected
#define LORAWAN_STATUS_QUEUE_FULL 0x07 				//Message queue is full, command rejected
#define LORAWAN_STATUS_LENGTH_ERROR 0x08 			//HCI message length is invalid or radio payload size is too large
#define LORAWAN_STATUS_NO_FACTORY_SETTINGS 0x09 	//Factory Settings EEPROM block missing or incompatible with current FW version
#define LORAWAN_STATUS_CHANNEL_BLOCKED 0x0A 		//Channel blocked by Duty Cycle
#define LORAWAN_STATUS_CHANNEL_NOT AVAILABLE 0x0B 	//No channel available (e.g. no channel defined for the configured spreading factor)

#define RADIO_CFG_ADAPTIVE_DATA_RATE 0x01;
#define RADIO_CFG_DUTY_CYCLE_CONTROL 0x02;
#define RADIO_CFG_CLASS_C 0x08;
#define RADIO_CFG_POWER_UP_IND 0x10;
#define RADIO_CFG_PRIVATE_NTW 0x20;
#define RADIO_CFG_EXTENDED_PKT 0x400;
#define RADIO_CFG_MAC_COMMAND_FORWARD 0x80;

#define APP_KEY_SIZE 16
#define APP_EUI_SIZE 8
#define DEVICE_EUI_SIZE 8
#define DEVICE_ADDRESS_SIZE 4
#define DEVICE_ID_SIZE 4

typedef struct {
	uint8_t DataRateIndex;
	uint8_t TXPwrLevel;
	uint8_t Options;
	uint8_t PowerSavingMode;
	uint8_t NumOfRetrans;
	uint8_t BandIndex;
	uint8_t HeadarMACCmdCap;
}RadioConfig_t;

typedef struct {
	uint8_t AppKey[16];
	uint8_t AppEUI[8];

}JoinParams_t;

typedef struct {
	uint8_t DeviceEUI[8];
	uint8_t DeviceAddress[4];
	uint8_t DeviceID[4];
}DeviceParams_t;

typedef struct {

}LoRaDeviceStatus_t;


TWiMOD_HCI_Message* HCICallback(TWiMOD_HCI_Message* rxMessage);
void Process_LoRaWAN_Message(TWiMOD_HCI_Message* rxMessage);
void Process_DeviceManagement_Message(TWiMOD_HCI_Message* rxMessage);


class LoRaDevice {
	public:
		LoRaDevice(UART_HandleTypeDef *Handler);
		~LoRaDevice();
		int Send(uint8_t *data,uint16_t size);
		void Process();
		int Connect();
		int Diconnect();
		int GetNtwkStatus();
		int Reset();
		LoRaDeviceStatus_t GetDeviceStatus();
		int SetRadioConfig(RadioConfig_t _RadioConfig);
		int GetRadioConfig();
		RadioConfig_t* GetCurrentRadioConfig();
		int SetJoinParams(JoinParams_t _JoinParams);
		void Wakeup();
		void SetPort(uint8_t NewPort);


	private:
		HCI *Hci;
		uint8_t Port = 0x02;
		RadioConfig_t RadioConfig;
		JoinParams_t JoinParams;
		DeviceParams_t DeviceParams;
		LoRaDeviceStatus_t status;
};


#endif /* LORA_H_ */
