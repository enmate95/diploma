/*
 * LORA.h
 *
 *  Created on: May 1, 2020
 *      Author: mate_pc
 */

#ifndef LORA_H_
#define LORA_H_

#include "HCI.h"
#include <string.h>
#include "RadioDevice.h"
#include "DataManager.h"

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

#define RADIO_CFG_ADAPTIVE_DATA_RATE 0x01
#define RADIO_CFG_DUTY_CYCLE_CONTROL 0x02
#define RADIO_CFG_CLASS_C 0x04
#define RADIO_CFG_POWER_UP_IND 0x08
#define RADIO_CFG_PRIVATE_NTW 0x10
#define RADIO_CFG_EXTENDED_PKT 0x20
#define RADIO_CFG_MAC_COMMAND_FORWARD 0x40

#define APP_KEY_SIZE 16
#define APP_EUI_SIZE 8
#define DEVICE_EUI_SIZE 8
#define DEVICE_ADDRESS_SIZE 4
#define DEVICE_ID_SIZE 4

namespace LoRa {

 class  HCIMessage{
 public:
 	void set(LoRaHCI::TWiMOD_HCI_Message *msg) {
 		this->msg = msg;
 	}
 	LoRaHCI::TWiMOD_HCI_Message* get() { return msg;}

 	HCIMessage(LoRaHCI::TWiMOD_HCI_Message *msg) {
 		set(msg);
 	}
 	HCIMessage(){}
 	HCIMessage(const HCIMessage & other) { *this = other;}
 	HCIMessage & operator=(const HCIMessage &other) {
 		if(this != &other) {
 			set(msg);
 		}
 		return *this;
 	}
 private:
 	LoRaHCI::TWiMOD_HCI_Message *msg;
 };

class LoRaRadioConfig{
public:
	LoRaRadioConfig()=default;
	~LoRaRadioConfig()=default;

	void setDataRateIndex(uint8_t DataRateIndex) {this->DataRateIndex = DataRateIndex;}
	uint8_t getDataRateIndex() {return DataRateIndex;}

	void setTXPwrLevel(uint8_t TXPwrLevel) {this->TXPwrLevel = TXPwrLevel;}
	uint8_t getTXPwrLevel() {return TXPwrLevel;}

	void setOptions(uint8_t Options) {this->Options = Options;}
	uint8_t getOptions() {return Options;}

	void setPowerSavingMode(uint8_t PowerSavingMode) {this->PowerSavingMode = PowerSavingMode;}
	uint8_t getPowerSavingMode() {return PowerSavingMode;}

	void setNumOfRetrans(uint8_t NumOfRetrans) {this->NumOfRetrans = NumOfRetrans;}
	uint8_t getNumOfRetrans() {return NumOfRetrans;}

	void setBandIndex(uint8_t BandIndex) {this->BandIndex = BandIndex;}
	uint8_t getBandIndex() {return BandIndex;}

	void setHeadarMACCmdCap(uint8_t HeadarMACCmdCap) {this->HeadarMACCmdCap = HeadarMACCmdCap;}
	uint8_t getHeadarMACCmdCap() {return HeadarMACCmdCap;}

	LoRaRadioConfig(const LoRaRadioConfig & other) { *this = other; }
	LoRaRadioConfig& operator=(const LoRaRadioConfig & other) {
		if(this != &other) {
			this-> DataRateIndex = other.DataRateIndex;
			this-> TXPwrLevel = other.TXPwrLevel;
			this-> Options = other.Options;
			this-> PowerSavingMode = other.PowerSavingMode;
			this-> NumOfRetrans = other.NumOfRetrans;
			this-> BandIndex = other.BandIndex;
			this-> HeadarMACCmdCap = other.HeadarMACCmdCap;
		}
		return *this;
	}

	static const uint8_t AdaptiveDataRateEnable = 0x01;
	static const uint8_t DutyCycleControlEnable = 0x02;
	static const uint8_t ClassCEnable = 0x04;
	static const uint8_t PowerUpIndicationEnable = 0x10;
	static const uint8_t PrivateNetworkEnable = 0x20;
	static const uint8_t ExtendedRfPacketEnable = 0x40;
	static const uint8_t RxMACCommandForwardingEnable = 0x80;
private:
	uint8_t DataRateIndex;
	uint8_t TXPwrLevel;
	uint8_t Options;
	uint8_t PowerSavingMode;
	uint8_t NumOfRetrans;
	uint8_t BandIndex;
	uint8_t HeadarMACCmdCap;
};

class LoRaJoinParams{
public:
	LoRaJoinParams() = default;
	~LoRaJoinParams() = default;

	void setAppKey(char *AppKey) { FillAppKey(AppKey);}
	void setAppEUI(char* AppEUI) { FillAppEUI(AppEUI);}
	char* getAppKey() {return AppKey;}
	char* getAppEUI() {return AppEUI;}

	LoRaJoinParams(const LoRaJoinParams & other) { *this = other; }
	LoRaJoinParams& operator=(const LoRaJoinParams & other) {
		if(this != &other) {
			memcpy(this->AppKey,other.AppKey,sizeof(this->AppKey));
			memcpy(this->AppEUI,other.AppEUI,sizeof(this->AppEUI));
		}
		return *this;
	}

private:
	bool FillAppKey(const char* AppKey) {
		uint8_t upper;
		uint8_t lower;

		for(int i = 0; i < static_cast<int>(strlen(AppKey)) - 1; i = i + 2) {
			upper = CheckChar(AppKey[i]) << 4;
			lower = CheckChar(AppKey[i+1]) & 0x0f;
			if(lower == 0xff || upper == 0xff)
				return false;
			this->AppKey[i/2] = upper | lower;
		}
		return true;
	}

	bool FillAppEUI(const char* AppEUI) {
			uint8_t upper;
			uint8_t lower;

			for(int i = 0; i < static_cast<int>(strlen(AppEUI)) - 1; i = i + 2) {
				upper = CheckChar(AppEUI[i]) << 4;
				lower = CheckChar(AppEUI[i+1]) & 0x0f;
				if(lower == 0xff || upper == 0xff)
					return false;
				this->AppEUI[i/2] = upper | lower;
			}
			return true;
		}

	uint8_t CheckChar(char c) {
		static char lookup_table[] = "0123456789abcdefABCDEF";
		for(int j = 0; j < static_cast<int>(strlen(lookup_table)); j++) {
			if(c == lookup_table[j]) {
				if(j < 10)
					return (uint8_t)(c - '0');
				if(j >= 10 && j < 16)
					return  (uint8_t)(c - 'a' + 10);
				if(j >= 16)
					return  (uint8_t)(c - 'A' + 10);
			}
		}
		return 0xff;
	}
private:
	char AppKey[16];
	char AppEUI[8];
};

class  LoRaDeviceParams{
public:
		void setDeviceEUI(char *DeviceEUI) { memcpy(this->DeviceEUI,DeviceEUI,sizeof(this->DeviceEUI));}
		void setDeviceAddress(char* DeviceAddress) { memcpy(this->DeviceAddress,DeviceAddress,sizeof(this->DeviceAddress));}
		void setDeviceID(char* DeviceID) { memcpy(this->DeviceID,DeviceID,sizeof(this->DeviceID));}
		char* getDeviceEUI() {return DeviceEUI;}
		char* getDeviceAddress() {return DeviceAddress;}
		char* getDeviceID() {return DeviceID;}
		LoRaDeviceParams() = default;
		LoRaDeviceParams(const LoRaDeviceParams & other) { *this = other; }
		LoRaDeviceParams & operator=(const LoRaDeviceParams & other) {
			if(this != &other) {
				memcpy(this->DeviceEUI,other.DeviceEUI,sizeof(this->DeviceEUI));
				memcpy(this->DeviceAddress,other.DeviceAddress,sizeof(this->DeviceAddress));
				memcpy(this->DeviceID,other.DeviceID,sizeof(this->DeviceID));
			}
			return *this;
		}
private:
	char DeviceEUI[8];
	char DeviceAddress[4];
	char DeviceID[4];
};

typedef std::function<void()> LORAPingCb_t;
typedef std::function<void(LoRaRadioConfig*)> LORAGetRadioConfigCb_t;
typedef std::function<void()> LORASetRadioConfigCb_t;
typedef std::function<void()> LORASetJoinParamsCb_t;
typedef std::function<void()> LORAGetNtwkStatusCb_t;
typedef std::function<void()> LORAConnectionCompletedCb_t;
typedef std::function<void()> LORADisconnectionCompletedCb_t;
typedef std::function<void(LoRaDeviceParams*)> LORAGetDeviceParamsCb_t;
typedef std::function<void(uint8_t*,uint16_t)> LORADownlinkEventCb_t;

enum {
	IDLE,
	SENT,
	ERROR,
	CHANNEL_BLOCKED,
	BUSY,
	NOT_ACTIVATED,
}UplinkMessageStatus_t;

enum {
	LORA_RADIO_CONFIG,
	LORA_DEVICE_PARAMS,
	LORA_UPLINK_STATUS,
}LoRaParam_t;

enum {
	LORA_GET_RADIO_CONFIG_CB,
	LORA_SET_RADIO_CONFIG_CB,
	LORA_SET_JOIN_PARAMS_CB,
	LORA_GET_NTWK_STATUS_CB,
	LORA_CONNECTION_COMPLETE_CB,
	LORA_DISCONNECTION_COMPLETE_CB,
	LORA_GET_DEVICE_PARAMS_CB,
	LORA_DOWNLNK_EVENT_CB,
	LORA_PING_CB,
}LoRaCb_t;

enum {
	CONNECT,
	DISCONNECT,
	GET_NETWORK_STATUS,
	SW_RESET,
	PING,
	SET_JOIN_PARAMS,
	SET_RADIO_CONFIG,
	GET_RADIO_CONFIG,
	WAKEUP,
}LoRaCmd_t;

class LoRaDevice :public RadioDevice {
	public:
		//constructors
		LoRaDevice(LoRaHCI::HCI & hci) : hci(hci){}
		~LoRaDevice() = default;

		void init(const Container & data);
		void setParameter(int param, const Container & value);
		Container getParameter(int param);
		void setCallback(int type, const Container & value);
		//while(validData) {callFunc()}
		void process();
		bool send(const Container & data);
		bool sendCmd(int cmd);
		void callHandler();
		void start();
		void stop();
private:
		bool Connect();
		bool Disconnect();
		bool GetNtwkStatus();
		bool SwReset();
		bool Ping();
		bool SetConnectionParams();
		bool SetRadioConfig();
		bool GetRadioConfig();
		void Wakeup();
		void HandleRxMessage(LoRaHCI::TWiMOD_HCI_Message *RxMessage);

	private:
		LoRa::LORAGetRadioConfigCb_t GetRadioConfigCallback;
		LoRa::LORASetRadioConfigCb_t SetRadioConfigCallback;
		LoRa::LORASetJoinParamsCb_t SetJoinParamsCallback;
		LoRa::LORAGetNtwkStatusCb_t GetNtwkStatusCallback;
		LoRa::LORAConnectionCompletedCb_t ConnectionCompletedCallback;
		LoRa::LORADisconnectionCompletedCb_t DisconnectionCompletedCallback;
		LoRa::LORAGetDeviceParamsCb_t GetDeviceParamsCallback;
		LoRa::LORADownlinkEventCb_t DownlinkEventCallback;
		LORAPingCb_t PingCallback;
		void Rxhandler();

		LoRaHCI::TWiMOD_HCI_Message TxMessage;
		LoRaHCI::HCI &hci;
		uint8_t Port = 0x02;
		LoRaRadioConfig RadioConfig;
		LoRaJoinParams JoinParams;
		LoRaDeviceParams DeviceParams;
		int UplinkStatus = IDLE;
		bool flag = false;
};
}

#endif /* LORA_H_ */
