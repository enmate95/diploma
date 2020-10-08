/*
 * LORA.cpp
 *
 *  Created on: May 2, 2020
 *      Author: mate_pc
 */

#include "LORA.h"

void LoRa::LoRaDevice::init(const Container & data) {
	DataManager<LoRaJoinParams> dm;
	JoinParams = dm.convertFromContainer(data);
}

void LoRa::LoRaDevice::setParameter(int param, const Container & data) {
	switch(param) {
		case LORA_RADIO_CONFIG: {
			DataManager<LoRaRadioConfig> dm;
			RadioConfig = dm.convertFromContainer(data);
		}
		break;

		case LORA_DEVICE_PARAMS: {
			DataManager<LoRaDeviceParams> dm;
			DeviceParams = dm.convertFromContainer(data);
		}
	default:
		break;
	}
}

Container LoRa::LoRaDevice::getParameter(int param) {
	switch(param) {
		case LORA_RADIO_CONFIG: {
			DataManager<LoRaRadioConfig> dm;
			return dm.convertFromData(RadioConfig);
		}
		break;

		case LORA_DEVICE_PARAMS: {
			DataManager<LoRaDeviceParams> dm;
			return dm.convertFromData(DeviceParams);
		}
		break;

		case LORA_UPLINK_STATUS: {
			DataManager<int> dm;
			return dm.convertFromData(UplinkStatus);
		}
		break;

		default:
			Container con;
			return con;
		break;
	}
}

void LoRa::LoRaDevice::setCallback(int type, const Container & value) {
	switch(type) {
		case LORA_GET_RADIO_CONFIG_CB: {
			DataManager<LORAGetRadioConfigCb_t> dm;
			GetRadioConfigCallback = dm.convertFromContainer(value);
		}
		break;

		case LORA_SET_RADIO_CONFIG_CB: {
			DataManager<LORASetRadioConfigCb_t> dm;
			SetRadioConfigCallback = dm.convertFromContainer(value);
		}
		break;

		case LORA_SET_JOIN_PARAMS_CB: {
			DataManager<LORASetJoinParamsCb_t> dm;
			SetJoinParamsCallback = dm.convertFromContainer(value);
		}
		break;

		case LORA_GET_NTWK_STATUS_CB: {
			DataManager<LORAGetNtwkStatusCb_t> dm;
			GetNtwkStatusCallback = dm.convertFromContainer(value);
		}
		break;

		case LORA_CONNECTION_COMPLETE_CB: {
			DataManager<LORAConnectionCompletedCb_t> dm;
			ConnectionCompletedCallback = dm.convertFromContainer(value);
		}
		break;

		case LORA_DISCONNECTION_COMPLETE_CB: {
			DataManager<LORADisconnectionCompletedCb_t> dm;
			DisconnectionCompletedCallback = dm.convertFromContainer(value);
		}
		break;

		case LORA_GET_DEVICE_PARAMS_CB: {
			DataManager<LORAGetDeviceParamsCb_t> dm;
			GetDeviceParamsCallback = dm.convertFromContainer(value);
		}
		break;

		case LORA_DOWNLNK_EVENT_CB: {
			DataManager<LORADownlinkEventCb_t> dm;
			DownlinkEventCallback = dm.convertFromContainer(value);
		}
		break;

		case LORA_PING_CB: {
			DataManager<LORAPingCb_t> dm;
			PingCallback = dm.convertFromContainer(value);
		}
		break;

	default:
		break;
	}
}

void LoRa::LoRaDevice::callHandler() {
	Rxhandler();
}

void LoRa::LoRaDevice::Rxhandler() {
	UartDMA *hciSerial;
	hciSerial = hci.GetSerial();
	if(hciSerial->uartHandleFLags()) {
		if(hciSerial->setLength()) {
			flag = true;
		}
	}
}

bool LoRa::LoRaDevice::send(const Container & data) {
	DataManager<HCIMessage> dm;
	HCIMessage message;
	message = dm.convertFromContainer(data);
	busy = true;
	return hci.SendMessage(*message.get());
}

bool LoRa::LoRaDevice::sendCmd(int cmd) {
	switch(cmd) {
	case CONNECT:
		if(Connect()) {
			return true;
		}
		return false;
		break;

	case DISCONNECT:
		if(Disconnect()) {
			return true;
		}
		return false;
		break;

	case GET_NETWORK_STATUS:
		if(GetNtwkStatus()) {
			return true;
		}
		return false;
		break;

	case SW_RESET:
		if(SwReset()) {
			return true;
		}
		return false;
		break;

	case PING:
		if(Ping()) {
			return true;
		}
		return false;
		break;

	case SET_JOIN_PARAMS:
		if(SetConnectionParams()) {
			return true;
		}
		return false;
		break;

	case SET_RADIO_CONFIG:
		if(SetRadioConfig()) {
			return true;
		}
		return false;
		break;

	case GET_RADIO_CONFIG:
		if(GetRadioConfig()) {
			return true;
		}
		return false;
		break;

	case WAKEUP:
		Wakeup();
		return true;
		break;

	default:
		return false;
		break;
	}
}

void LoRa::LoRaDevice::process() {
	if(flag) {
		flag = false;
		if(hci.Process()) {
			HandleRxMessage(hci.GetRxMessage());
		}
	}
}


bool LoRa::LoRaDevice::LoRaDevice::Connect() {
	busy = true;
	TxMessage.SapID = LORAWAN_SAP_ID;
	TxMessage.MsgID = LORAWAN_MSG_JOIN_NETWORK_REQ;
	TxMessage.Length = 0;
	return hci.SendMessage(TxMessage);
}

bool LoRa::LoRaDevice::Disconnect() {
	busy = true;
	TxMessage.SapID = LORAWAN_SAP_ID;
	TxMessage.MsgID = LORAWAN_MSG_DEACTIVATE_DEVICE_REQ;
	TxMessage.Length = 0;
	return hci.SendMessage(TxMessage);
}

bool LoRa::LoRaDevice::GetNtwkStatus() {
	busy = true;
	TxMessage.SapID = LORAWAN_SAP_ID;
	TxMessage.MsgID = LORAWAN_MSG_GET_NWK_STATUS_REQ;
	TxMessage.Length = 0;
	return hci.SendMessage(TxMessage);
}

bool LoRa::LoRaDevice::SwReset() {
	busy = true;
	TxMessage.SapID = DEVMGMT_SAP_ID;
	TxMessage.MsgID = DEVMGMT_MSG_RESET_REQ;
	TxMessage.Length = 0;
	return hci.SendMessage(TxMessage);
}

bool LoRa::LoRaDevice::Ping() {
	TxMessage.SapID = DEVMGMT_SAP_ID;
	TxMessage.MsgID = DEVMGMT_MSG_PING_REQ;
	TxMessage.Length = 0;
	return hci.SendMessage(TxMessage);
}

void LoRa::LoRaDevice:: start() {
	hci.Enable();
}
void LoRa::LoRaDevice::stop() {
	hci.Disable();
}

void LoRa::LoRaDevice::HandleRxMessage(LoRaHCI::TWiMOD_HCI_Message *RxMessage) {
	switch(RxMessage->MsgID) {
		case LORAWAN_MSG_JOIN_NETWORK_IND:
			if(RxMessage->Payload[0] == 0x00 || RxMessage->Payload[0] == 0x01) {	//join success with/without info attached
				ConnectionCompletedCallback();
				busy = false;
			}
		break;

		case LORAWAN_MSG_GET_NWK_STATUS_RSP:
			if(RxMessage->Payload[0] == LORAWAN_STATUS_OK) {
				if(RxMessage->Payload[1] == 0x02) {									//OTAA active
					ConnectionCompletedCallback();
				}
				else {
					GetNtwkStatusCallback();
				}
				busy = false;
			}
		break;

		case LORAWAN_MSG_SET_JOIN_PARAM_RSP:
			if(RxMessage->Payload[0] == LORAWAN_STATUS_OK) {
				SetJoinParamsCallback();
				busy = false;
			}
		break;

		case LORAWAN_MSG_DEACTIVATE_DEVICE_RSP:
			if(RxMessage->Payload[0] == LORAWAN_STATUS_OK) {
				DisconnectionCompletedCallback();
				busy = false;
			}
		break;

		case LORAWAN_MSG_GET_RSTACK_CONFIG_RSP:
			if(RxMessage->Payload[0] == LORAWAN_STATUS_OK) {
				RadioConfig.setDataRateIndex(RxMessage->Payload[1]);
				RadioConfig.setTXPwrLevel(RxMessage->Payload[2]);
				RadioConfig.setOptions(RxMessage->Payload[3]);
				RadioConfig.setPowerSavingMode(RxMessage->Payload[4]);
				RadioConfig.setNumOfRetrans(RxMessage->Payload[5]);
				RadioConfig.setBandIndex(RxMessage->Payload[6]);
				RadioConfig.setHeadarMACCmdCap(RxMessage->Payload[7]);
				GetRadioConfigCallback(&RadioConfig);
				busy = false;
			}
		break;

		case LORAWAN_MSG_SET_RSTACK_CONFIG_RSP:
			if(RxMessage->Payload[0] == LORAWAN_STATUS_OK) {
				SetRadioConfigCallback();
				busy = false;
			}
		break;

		case LORAWAN_MSG_SEND_UDATA_RSP:
			switch(RxMessage->Payload[0]) {
				case LORAWAN_STATUS_OK:
					UplinkStatus = SENT;
				break;

				case LORAWAN_STATUS_DEVICE_NOT_ACTIVATED:
					UplinkStatus = NOT_ACTIVATED;
				break;

				case LORAWAN_STATUS_DEVICE_BUSY:
					UplinkStatus = BUSY;
				break;

				case LORAWAN_STATUS_CHANNEL_BLOCKED:
					UplinkStatus = CHANNEL_BLOCKED;
				break;

				default:
					UplinkStatus =ERROR;
				break;
				busy = false;
			}
		break;

		case DEVMGMT_MSG_PING_RSP:
				PingCallback();
			break;

		case LORAWAN_MSG_RECV_UDATA_IND:
			event = true;
		break;
	}
}

bool LoRa::LoRaDevice::SetRadioConfig() {
	busy = true;
	TxMessage.SapID = LORAWAN_SAP_ID;
	TxMessage.MsgID = LORAWAN_MSG_SET_RSTACK_CONFIG_REQ;
	TxMessage.Length = 7;
	TxMessage.Payload[0] = RadioConfig.getDataRateIndex();
	TxMessage.Payload[1] = RadioConfig.getTXPwrLevel();
	TxMessage.Payload[2] = RadioConfig.getOptions();
	TxMessage.Payload[3] = RadioConfig.getPowerSavingMode();
	TxMessage.Payload[4] = RadioConfig.getNumOfRetrans();
	TxMessage.Payload[5] = RadioConfig.getBandIndex();
	TxMessage.Payload[6] = RadioConfig.getHeadarMACCmdCap();
	return hci.SendMessage(TxMessage);
}

bool LoRa::LoRaDevice::GetRadioConfig() {
	busy = true;
	TxMessage.SapID = LORAWAN_SAP_ID;
	TxMessage.MsgID = LORAWAN_MSG_GET_RSTACK_CONFIG_REQ;
	TxMessage.Length = 0;
	return hci.SendMessage(TxMessage);
}

bool LoRa::LoRaDevice::SetConnectionParams() {
	busy = true;
	TxMessage.SapID = LORAWAN_SAP_ID;
	TxMessage.MsgID = LORAWAN_MSG_SET_JOIN_PARAM_REQ;
	TxMessage.Length  = 24;
	memcpy(TxMessage.Payload,JoinParams.getAppEUI(),APP_EUI_SIZE);
	memcpy(TxMessage.Payload + 8,JoinParams.getAppKey() ,APP_KEY_SIZE);
	return hci.SendMessage(TxMessage);
}

void LoRa::LoRaDevice::Wakeup() {
	hci.Wakeup();
}

