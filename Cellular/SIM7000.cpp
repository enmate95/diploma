#include "SIM7000.h"
#include "DataManager.h"

void Cellular::SIM7000::init(const Container & data) {
	mode = DataManager<Cellular::SIM7000Mode_t>::convertFromContainer(data);
}

void Cellular::SIM7000::setParameter(int param, const Container & value) {
	switch(param) {
		case SIM7000_DEVICE_PARAMS:
			params = &DataManager<Cellular::SIM7000Params>::convertFromContainer(value);
		break;

		case SIM7000_ROLE:
			role = DataManager<Cellular::SIM7000Role_t>::convertFromContainer(value);
		break;

		case SIM7000_PROTOCOL:
			protocol = DataManager<Cellular::SIM7000Protocol_t>::convertFromContainer(value);
		break;

		case SIM7000_SIDE_MODE:
			sidemode = DataManager<Cellular::SIM7000SideMode_t>::convertFromContainer(value);
		break;
	}
}

Container Cellular::SIM7000::getParameter(int param) {
	switch(param) {
		case SIM7000_DEVICE_PARAMS:
			return DataManager<Cellular::SIM7000Params>::convertFromData(*params);
		break;

		case SIM7000_ROLE:
			return DataManager<Cellular::SIM7000Role_t>::convertFromData(role);
		break;

		case SIM7000_PROTOCOL:
			return DataManager<Cellular::SIM7000Protocol_t>::convertFromData(protocol);
		break;

		case SIM7000_SIDE_MODE:
			return DataManager<Cellular::SIM7000SideMode_t>::convertFromData(sidemode);
		break;
	}
}

void Cellular::SIM7000::setCallback(int type, const Container & value) {
	switch(type) {
		case SIM7000_GET_IP_CB: {
			SIM7000GetIPCallback = DataManager<Cellular::SIM7000GetIPCb_t>::convertFromContainer(value);
		}
		break;

		case SIM7000_IS_STATUS_OK_CB: {
			SIM7000IsStatusOKCallback = DataManager<Cellular::SIM7000IsStatusOKCb_t>::convertFromContainer(value);
		}
		break;

		case SIM7000_IS_PIN_CB: {
			SIM7000IsPINCallback = DataManager<Cellular::SIM7000IsPINCb_t>::convertFromContainer(value);
		}
		break;

		case SIM7000_TCP_UDP_DATA_RECEIVED_CB: {
			SIM7000TCP_UDP_DataReceivedEventCallback = DataManager<Cellular::SIM7000TCP_UDP_DataReceivedEventCb_t>::convertFromContainer(value);
		}
		break;

		case SIM7000_HTTP_GET_RECEIVED_CB: {
			SIM7000GETResultCallback = DataManager<Cellular::SIM7000GETResultCb_t>::convertFromContainer(value);
		}
		break;

		case SIM7000_HTTP_POST_SENT_CB: {
			SIM7000POSTSessionDoneCallback = DataManager<Cellular::SIM7000POSTSessionDoneCb_t>::convertFromContainer(value);
		}
		break;

		case SIM7000_TCP_CONNECTED_TO_SERVER_CB: {
			SIM7000ConnectedToTCPServerCallback = DataManager<Cellular::SIM7000ConnectedToTCPServerCb_t>::convertFromContainer(value);
		}
		break;

		case SIM7000_TCP_DISCONNECTED_FROM_SERVER_CB: {
			SIM7000DisconnectedFromTCPServerCallback = DataManager<Cellular::SIM7000DisconnectedFromTCPServerCb_t>::convertFromContainer(value);
		}
		break;

		case SIM7000_TCP_SERVER_OPENED_CB: {
			SIM7000TCPServerOpenedCallback = DataManager<Cellular::SIM7000TCPServerOpenedCb_t>::convertFromContainer(value);
		}
		break;

		case SIM7000_TCP_SERVER_CLOSED_CB: {
			SIM7000TCPServerClosedCallback = DataManager<Cellular::SIM7000TCPServerClosedCb_t>::convertFromContainer(value);
		}
		break;

		case SIM7000_TCP_CLIENT_CONNECTED_CB: {
			SIM7000TCPClientConnectedCallback = DataManager<Cellular::SIM7000TCPClientConnectedCb_t>::convertFromContainer(value);
		}
		break;

		case SIM7000_TCP_CLIENT_DISCONNECTED_CB: {
			SIM7000TCPClientDisconnectedCallback = DataManager<Cellular::SIM7000TCPClientDisconnectedCb_t>::convertFromContainer(value);
		}
		break;

		case SIM7000_GPRS_IS_CONNECTED_CB: {
			SIM7000IsGPRSConnectedCallback = DataManager<Cellular::SIM7000IsGPRSConnectedCb_t>::convertFromContainer(value);
		}
		break;
		}
}

void Cellular::SIM7000::ProcessDone() {
	serial.stopDMA();
	serial.clearBuffer();
	serial.startDMA();
	ClearBuff();
	flag = false;
	busy = false;
}

void Cellular::SIM7000::process() {
	if(flag) {
		serial.copyBuffer(&raw);

		if(role == HTTP) {
			HandleHTTP();
		}
		else {
			switch(protocol) {
				case TCP_CLIENT:
					HandleTCPClient();
				break;

				case TCP_SERVER:
					HandleTCPServer();
				break;

				case UDP:
					HandleUDP();
				break;
			}
		}
	}
}

void Cellular::SIM7000::HandleTCPClient() {
	char* ptr;
	static bool sent = false;
	switch(state) {
		case SIM7000CommandState::SET_PIN:
			if(strstr((char*)raw.data,"READY") != nullptr) {
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
		break;

		case SIM7000CommandState::TCP_UDP_CONNECT_NETWORK:
			GetIP();
			state = SIM7000CommandState::GET_IP;
		break;

		case SIM7000CommandState::IS_PIN:
			if(strstr((char*)raw.data,"READY") != nullptr) {
				if(SIM7000IsPINCallback != nullptr)
					SIM7000IsPINCallback(false);
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
			else if(strstr((char*)raw.data,"SIM PIN") != nullptr){
				if(SIM7000IsPINCallback != nullptr)
					SIM7000IsPINCallback(true);
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
		break;

		case SIM7000CommandState::GET_GPRS_STATE:
			ptr = strstr((char*)raw.data,"+CGATT:");
			if(ptr != nullptr) {
				ptr += 8;
				int nwState = atoi(ptr);
				if(nwState) {
					if(SIM7000IsGPRSConnectedCallback != nullptr)
						SIM7000IsGPRSConnectedCallback(true);
				}
				else {
					if(SIM7000IsGPRSConnectedCallback != nullptr)
						SIM7000IsGPRSConnectedCallback(false);
				}
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
		break;

		case SIM7000CommandState::GET_IP:
			ptr = strstr((char*)raw.data,"+CIFSREX");
			if(ptr != nullptr) {
				ptr += 10;
				char ip[20];
				char* start = ptr;
				int ipLength = 0;
				while((*ptr == '.') || (IsNumber(*ptr))) {
					ptr++;
					ipLength++;
				}
				memcpy(ip,start,ipLength);
				params->setIP(ip);
				if(SIM7000GetIPCallback != nullptr)
					SIM7000GetIPCallback(params->getIP());
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
		break;

		case SIM7000CommandState::DISABLE_ECHO:
			ProcessDone();
			state = SIM7000CommandState::IDLE;
		break;

		case SIM7000CommandState::TCP_UDP_SEND:
			ptr = strstr((char*)raw.data,"SEND OK");
			if(ptr != nullptr){
				ptr = strstr((char*)raw.data,"+IPD");
				if(ptr != nullptr) {
					int length = atoi(ptr + 5);
					while(*ptr != ':') {
						ptr++;
					}
					ptr++;
					if(SIM7000TCP_UDP_DataReceivedEventCallback != nullptr)
						SIM7000TCP_UDP_DataReceivedEventCallback(ptr,length,0);
				}
				ProcessDone();
				state = SIM7000CommandState::IDLE;
				sent = false;
			}
			else if((strstr((char*)raw.data,">") != nullptr) && (!sent)) {
				serial.send((uint8_t*)toSend->getData(),toSend->getLength());
				sent = true;
			}
		break;

		case SIM7000CommandState::TCP_CONNECT_SERVER:
			if(strstr((char*)raw.data,"CONNECT") != nullptr) {
				if(SIM7000ConnectedToTCPServerCallback != nullptr)
					SIM7000ConnectedToTCPServerCallback();
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
		break;

		case SIM7000CommandState::TCP_DISCONNECT_SERVER:
			if(strstr((char*)raw.data,"CLOSE") != nullptr) {
				if(SIM7000DisconnectedFromTCPServerCallback != nullptr)
					SIM7000DisconnectedFromTCPServerCallback();
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
		break;

		case SIM7000CommandState::IDLE:
			ptr = strstr((char*)raw.data,"CLOSED");
			if(ptr != nullptr) {
				if(SIM7000DisconnectedFromTCPServerCallback != nullptr)
					SIM7000DisconnectedFromTCPServerCallback();
			}
			ptr = strstr((char*)raw.data,"+IPD");
			if(ptr != nullptr) {
				int length = atoi(ptr + 5);
				while(*ptr != ':') {
					ptr++;
				}
				ptr++;
				if(SIM7000TCP_UDP_DataReceivedEventCallback != nullptr)
					SIM7000TCP_UDP_DataReceivedEventCallback(ptr,length,0);
			}
			ProcessDone();
			state = SIM7000CommandState::IDLE;
		break;

		default:
			if(strstr((char*)raw.data,"OK") != nullptr) {
				if(SIM7000IsStatusOKCallback != nullptr)
					SIM7000IsStatusOKCallback(true);
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
			else if(strstr((char*)raw.data,"ERROR") != nullptr){
				if(SIM7000IsStatusOKCallback != nullptr)
					SIM7000IsStatusOKCallback(false);
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
		break;
	}
}

void Cellular::SIM7000::HandleTCPServer() {
	char* ptr;
	bool sent = false;
	switch(state) {
		case SIM7000CommandState::SET_PIN:
			if(strstr((char*)raw.data,"READY") != nullptr) {
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
		break;

		case SIM7000CommandState::TCP_UDP_CONNECT_NETWORK:
			GetIP();
			state = SIM7000CommandState::GET_IP;
		break;

		case SIM7000CommandState::IS_PIN:
			if(strstr((char*)raw.data,"READY") != nullptr) {
				if(SIM7000IsPINCallback != nullptr)
					SIM7000IsPINCallback(false);
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
			else if(strstr((char*)raw.data,"SIM PIN") != nullptr){
				if(SIM7000IsPINCallback != nullptr)
					SIM7000IsPINCallback(true);
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
		break;

		case SIM7000CommandState::GET_GPRS_STATE:
			ptr = strstr((char*)raw.data,"+CGATT:");
			if(ptr != nullptr) {
				ptr += 8;
				int nwState = atoi(ptr);
				if(nwState) {
					if(SIM7000IsGPRSConnectedCallback != nullptr)
						SIM7000IsGPRSConnectedCallback(true);
				}
				else {
					if(SIM7000IsGPRSConnectedCallback != nullptr)
						SIM7000IsGPRSConnectedCallback(false);
				}
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
		break;

		case SIM7000CommandState::GET_IP:
			ptr = strstr((char*)raw.data,"+CIFSREX");
			if(ptr != nullptr) {
				ptr += 10;
				char ip[20];
				char* start = ptr;
				int ipLength = 0;
				while((*ptr == '.') || (IsNumber(*ptr))) {
					ptr++;
					ipLength++;
				}
				memcpy(ip,start,ipLength);
				params->setIP(ip);
				if(SIM7000GetIPCallback != nullptr)
					SIM7000GetIPCallback(params->getIP());
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
		break;

		case SIM7000CommandState::DISABLE_ECHO:
			ProcessDone();
			state = SIM7000CommandState::IDLE;
		break;

		case SIM7000CommandState::TCP_UDP_SEND:
			ptr = strstr((char*)raw.data,"SEND OK");
			if(ptr != nullptr){
				ptr = strstr((char*)raw.data,"+RECEIVE");
				if(ptr != nullptr) {
					int length = atoi(ptr + 11);
					int client = atoi(ptr + 9);
					while(*ptr != ':') {
						ptr++;
					}
					ptr++;
					if(SIM7000TCP_UDP_DataReceivedEventCallback != nullptr)
						SIM7000TCP_UDP_DataReceivedEventCallback(ptr,length,client);
					}
				else {
					ProcessDone();
					state = SIM7000CommandState::IDLE;
					sent = false;
				}
			}
			else if((strstr((char*)raw.data,">") != nullptr) && (!sent)) {
				serial.send((uint8_t*)toSend->getData(),toSend->getLength());
				sent = true;
			}
		break;

		case SIM7000CommandState::TCP_OPEN_SERVER:
			if(strstr((char*)raw.data,"SERVER OK") != nullptr) {
				if(SIM7000TCPServerOpenedCallback != nullptr)
					SIM7000TCPServerOpenedCallback();
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
		break;

		case SIM7000CommandState::TCP_CLOSE_SERVER:
			if(strstr((char*)raw.data,"SERVER CLOSE") != nullptr) {
				if(SIM7000TCPServerClosedCallback != nullptr)
					SIM7000TCPServerClosedCallback();
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
		break;

		case SIM7000CommandState::IDLE:
			ptr = strstr((char*)raw.data,",REMOTE");
			if(ptr != nullptr) {
				if(SIM7000TCPClientConnectedCallback != nullptr)
					SIM7000TCPClientConnectedCallback(atoi(ptr--));
			}

			ptr = strstr((char*)raw.data,",CLOSED");
			if(ptr != nullptr) {
				if(SIM7000TCPClientDisconnectedCallback != nullptr)
					SIM7000TCPClientDisconnectedCallback(atoi(ptr--));
			}

			ptr = strstr((char*)raw.data,"+RECEIVE");
			if(ptr != nullptr) {
				int length = atoi(ptr + 11);
				int client = atoi(ptr + 9);
				while(*ptr != ':') {
					ptr++;
				}
				ptr++;
				if(SIM7000TCP_UDP_DataReceivedEventCallback != nullptr)
					SIM7000TCP_UDP_DataReceivedEventCallback(ptr,length,client);
			}
			ProcessDone();
			state = SIM7000CommandState::IDLE;
		break;

		default:
			if(strstr((char*)raw.data,"OK") != nullptr) {
				if(SIM7000IsStatusOKCallback != nullptr)
					SIM7000IsStatusOKCallback(true);
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
			else if(strstr((char*)raw.data,"ERROR") != nullptr){
				if(SIM7000IsStatusOKCallback != nullptr)
					SIM7000IsStatusOKCallback(false);
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
		break;
	}
}

void Cellular::SIM7000::HandleUDP() {
//TODO:
}

void Cellular::SIM7000::HandleHTTP() {
	char* ptr;
	switch(state) {
		case SIM7000CommandState::HTTP_START_GET:
			if(strstr((char*)raw.data,"+HTTPACTION: 0") != nullptr) {
				serial.send((uint8_t*)"AT+HTTPREAD\r\n",13);
				state = SIM7000CommandState::HTTP_READ_GET;
			}
		break;

		case SIM7000CommandState::HTTP_READ_GET:
			ptr = strstr((char*)raw.data,"+HTTPREAD:");
			if(ptr != nullptr) {
				ptr += 11;
				int length = atoi(ptr);
				while(IsNumber(*ptr)) {
					ptr++;
				}
				if(SIM7000GETResultCallback != nullptr)
					SIM7000GETResultCallback(ptr,length);
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
		break;

		case SIM7000CommandState::HTTP_SET_POST_DATA:
			if(strstr((char*)raw.data,"DOWNLOAD") != nullptr) {
				serial.send((uint8_t*)toSend->getData(),toSend->getLength());
				state = SIM7000CommandState::HTTP_START_POST;
			}
		break;

		case SIM7000CommandState::HTTP_START_POST:
			if(strstr((char*)raw.data,"OK") != nullptr) {
				serial.send((uint8_t*)"AT+HTTPACTION=1\r\n",17);
				state = SIM7000CommandState::HTTP_POST_DONE;
			}
		break;

		case SIM7000CommandState::HTTP_POST_DONE:
			if(strstr((char*)raw.data,"+HTTPACTION: 1") != nullptr) {
				if(SIM7000POSTSessionDoneCallback != nullptr)
					SIM7000POSTSessionDoneCallback();
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
		break;

		case SIM7000CommandState::SET_PIN:
			if(strstr((char*)raw.data,"READY") != nullptr) {
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
		break;

		case SIM7000CommandState::IS_PIN:
			if(strstr((char*)raw.data,"READY") != nullptr) {
				if(SIM7000IsPINCallback != nullptr)
					SIM7000IsPINCallback(false);
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
			else if(strstr((char*)raw.data,"SIM PIN") != nullptr){
				if(SIM7000IsPINCallback != nullptr)
					SIM7000IsPINCallback(true);
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
		break;

		case SIM7000CommandState::GET_GPRS_STATE:
			ptr = strstr((char*)raw.data,"+CGATT:");
			if(ptr != nullptr) {
				ptr += 8;
				int nwState = atoi(ptr);
				if(nwState) {
					if(SIM7000IsGPRSConnectedCallback != nullptr)
						SIM7000IsGPRSConnectedCallback(true);
				}
				else {
					if(SIM7000IsGPRSConnectedCallback != nullptr)
						SIM7000IsGPRSConnectedCallback(false);
				}
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
		break;

		case SIM7000CommandState::GET_IP:
			ptr = strstr((char*)raw.data,"+SAPBR");
			if(ptr != nullptr) {
				char ip[30];
				int index = 0;
				ptr += 12;
				while(*ptr != '\"') {
					ptr++;
					index++;
					ip[index] = *ptr;
				}
				ip[index] = '\0';
				params->setIP(ip);
				if(SIM7000GetIPCallback != nullptr)
					SIM7000GetIPCallback(params->getIP());
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
		break;

		case SIM7000CommandState::DISABLE_ECHO:
			ProcessDone();
			state = SIM7000CommandState::IDLE;
		break;

		default:
			if(strstr((char*)raw.data,"OK") != nullptr) {
				if(SIM7000IsStatusOKCallback != nullptr)
					SIM7000IsStatusOKCallback(true);
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
			else if(strstr((char*)raw.data,"ERROR") != nullptr){
				if(SIM7000IsStatusOKCallback != nullptr)
					SIM7000IsStatusOKCallback(false);
				ProcessDone();
				state = SIM7000CommandState::IDLE;
			}
		break;
	}
}

bool Cellular::SIM7000::IsNumber(char c) {
	const char lookup[] = "0123456789";
	for(int i = 0; i < strlen(lookup); i++) {
		if(c == lookup[i])
			return true;
	}
	return false;
}

bool Cellular::SIM7000::send(const Container & data) {
	toSend = &DataManager<SIM7000Data>::convertFromContainer(data);
	busy = true;
	char send[30] = "AT+CIPSEND=";
	char size[10] = "";
	char client[10] ="";

	sprintf(client,"%d",toSend->getClient());
	sprintf(size,"%d",toSend->getLength());

	switch(role) {
		case TCP:
			state = SIM7000CommandState::TCP_UDP_SEND;
			if(protocol == TCP_CLIENT) {
				strcat(send,size);
				strcat(send,"\r\n");
				serial.send((uint8_t*)send,strlen(send));
			}
			else {
				strcat(send,client);
				strcat(send,",");
				strcat(send,size);
				strcat(send,"\r\n");
				serial.send((uint8_t*)send,strlen(send));
			}
		break;

		case UDP:
			state = SIM7000CommandState::TCP_UDP_SEND;
			strcat(send,size);
			strcat(send,"\r\n");
			serial.send((uint8_t*)send,strlen(send));
		break;

		case HTTP:
			if(protocol == HTTP_GET) {
				state = SIM7000CommandState::HTTP_START_GET;
				HTTPStartGETSession();
			}
			else {
				state = SIM7000CommandState::HTTP_SET_POST_DATA;
				HTTPSetPOSTData();
			}
		break;
	}
}

bool Cellular::SIM7000::sendCmd(int cmd) {
	busy = true;
	switch(cmd) {
		case SIM7000_SET_PIN:
			state = SIM7000CommandState::SET_PIN;
			SetPIN();
		break;

		case SIM7000_DISABLE_ECHO:
			state = SIM7000CommandState::DISABLE_ECHO;
			DisableEcho();
		break;

		case SIM7000_SET_RX_DATA_FORMAT:
			state = SIM7000CommandState::SET_RX_DATA_FORMAT;
			SetRxDataFormat();
		break;

		case SIM7000_IS_PIN_NEEDED:
			state = SIM7000CommandState::IS_PIN;
			GetPIN();
		break;

		case SIM7000_SET_MODE:
			state = SIM7000CommandState::SET_MODE;
			SetMode();
		break;

		case SIM7000_GET_IP:
			state = SIM7000CommandState::GET_IP;
			GetIP();
		break;

		case SIM7000_DISABLE_MULTIPLE_CONN:
			state = SIM7000CommandState::DISABLE_MULTIPLE_CONN;
			DisableMultipleConnection();
		break;

		case SIM7000_ENABLE_MULTIPLE_CONN:
			state = SIM7000CommandState::ENABLE_MULTIPLE_CONN;
			EnableMultipleConnection();
		break;

		case SIM7000_TCP_CONNECT_TO_SERVER:
			state = SIM7000CommandState::TCP_CONNECT_SERVER;
			TCPConnectServer();
		break;

		case SIM7000_TCP_DISCONNECT_FROM_SERVER:
			state = SIM7000CommandState::TCP_DISCONNECT_SERVER;
			TCPDisconnectFromServer();
		break;

		case SIM7000_TCP_OPEN_SERVER:
			state = SIM7000CommandState::TCP_OPEN_SERVER;
			TCPOpenServer();
		break;

		case SIM7000_TCP_CLOSE_SERVER:
			state = SIM7000CommandState::TCP_CLOSE_SERVER;
			TCPCloseServer();
		break;

		case SIM7000_TCP_UDP_CONNECT_NETWORK:
			state = SIM7000CommandState::TCP_UDP_CONNECT_NETWORK;
			TCPConnectToNetwork();
		break;

		case SIM7000_TCP_UDP_DISCONNECT_NETWORK:
			state = SIM7000CommandState::TCP_UDP_DISCONNECT_NETWORK;
			TCPDisconnectFromNetwork();
		break;

		case SIM7000_TCP_UDP_START_TASK:
			state = SIM7000CommandState::TCP_UDP_START_TASK;
			TCPStartTask();
		break;

		case SIM7000_GET_GPRS_STATUS:
			state = SIM7000CommandState::GET_GPRS_STATE;
			GetGPRSState();
		break;

		case SIM7000_HTTP_CONFIG_BEARER:
			state = SIM7000CommandState::HTTP_BEARER_CONFIG;
			HTTPBearerConfigure();
		break;

		case SIM7000_HTTP_OPEN_BEARER:
			state = SIM7000CommandState::HTTP_BEARER_OPEN;
			HTTPBearerOpen();
		break;

		case SIM7000_HTTP_CLOSE_BEARER:
			state = SIM7000CommandState::HTTP_BEARER_CLOSE;
			HTTPBearerClose();
		break;

		case SIM7000_HTTP_INIT:
			state = SIM7000CommandState::HTTP_START;
			HTTPInit();
		break;

		case SIM7000_HTTP_SET_CID:
			state = SIM7000CommandState::HTTP_SET_CID;
			HTTPSetCID();
		break;

		case SIM7000_HTTP_SET_URL:
			state = SIM7000CommandState::HTTP_SET_URL;
			HTTPSetURL();
		break;

		case SIM7000_HTTP_TERMINATE:
			state = SIM7000CommandState::HTTP_TERMINATE;
			HTTPTerminate();
		break;

		case SIM7000_UDP_START:
			state = SIM7000CommandState::UDP_START;
			UDPStart();
		break;
	}
}

void Cellular::SIM7000::callHandler() {
	if(serial.uartHandleFLags()) {
		if(serial.setLength()) {
			flag = true;
		}
	}
}

void Cellular::SIM7000::SetRxDataFormat() {
	serial.send((uint8_t*)"AT+CIPHEAD=1\r\n",14);
}

void Cellular::SIM7000::DisableEcho() {
	serial.send((uint8_t*)"ATE0\r\n",6);
}

void Cellular::SIM7000::SetSideMode() {
	if(sidemode == NB_IOT) {
		serial.send((uint8_t*)"AT+CMNB=1\r\n",11);
	}
	else if(sidemode == CAT_M){
		serial.send((uint8_t*)"AT+CMNB=2\r\n",11);
	}
	else {
		serial.send((uint8_t*)"AT+CMNB=3\r\n",11);
	}
}

void Cellular::SIM7000::SetPIN() {
	char buffer[50] = "AT+CPIN=";
	char pin[10];
	sprintf(pin,"%d\r\n",params->getPIN());
	strcat(buffer,pin);

	serial.send((uint8_t*)buffer,strlen(buffer));
}

void Cellular::SIM7000::GetPIN() {
	serial.send((uint8_t*)"AT+CPIN?\r\n",10);
}

void Cellular::SIM7000::GetGPRSState() {
	//serial.send((uint8_t*)"AT+CGATT?\r\n",11);
	serial.send((uint8_t*)"AT+CGATT?\r\n",11);
}

void Cellular::SIM7000::SetMode() {
	switch(mode) {
	case Automatic:
		serial.send((uint8_t*)"AT+CNMP=2\r\n",11);
	break;

	case LTE:
		serial.send((uint8_t*)"AT+CNMP=38\r\n",11);
	break;

	case GSM:
		serial.send((uint8_t*)"AT+CNMP=13\r\n",11);
	break;

	case GSM_LTE:
		serial.send((uint8_t*)"AT+CNMP=51\r\n",11);
	break;
	}
}

void Cellular::SIM7000::UDPStart() {
	//AT+CIPSTART="UDP","ip","port"
	char buffer[100] = "AT+CIPSTART=\"";
	char port[10] = "";
	sprintf(port,"%d\"\r\n",params->getPort());
	strcat(buffer, "UDP\",\"");
	strcat(buffer, params->getServerIP());
	strcat(buffer, "\",\"");
	strcat(buffer, port);
	serial.send((uint8_t*)buffer,strlen(buffer));
}

void Cellular::SIM7000::start() {
	serial.startDMA();
}

void Cellular::SIM7000::stop() {
	serial.stopDMA();
}

void Cellular::SIM7000::ClearBuff() {
	for(uint32_t i = 0; i < raw.length;i++)
		raw.data[i] = 0;
	raw.length = 0;
}

void Cellular::SIM7000::GetIP() {
	if(role == HTTP) {
		//AT+SAPBR=2,1
		serial.send((uint8_t*)"AT+SAPBR=2,1",12);
	}
	else {
		//AT+CIFSR
		serial.send((uint8_t*)"AT+CIFSREX\r\n",12);
	}
}

void Cellular::SIM7000::EnableMultipleConnection() {
	//AT+CIPMUX=1
	serial.send((uint8_t*)"AT+CIPMUX=1\r\n",13);
}

void Cellular::SIM7000::DisableMultipleConnection() {
	//AT+CIPMUX=0
	serial.send((uint8_t*)"AT+CIPMUX=0\r\n",13);
}

void Cellular::SIM7000::TCPConnectServer() {
	//AT+CIPSTART="TCP","IP",port
	char buffer[100] = "AT+CIPSTART=\"";
	char port[10] = "";
	sprintf(port,"%d\"\r\n",params->getPort());
	strcat(buffer, "TCP\",\"");
	strcat(buffer, params->getServerIP());
	strcat(buffer, "\",\"");
	strcat(buffer, port);
	serial.send((uint8_t*)buffer,strlen(buffer));
}

void Cellular::SIM7000::TCPDisconnectFromServer() {
	//AT+CIPCLOSE
	serial.send((uint8_t*)"AT+CIPCLOSE\r\n",13);
}

void Cellular::SIM7000::TCPDisconnectFromNetwork() {
	//"AT+CIPSHUT\r\n", wait for OK
	serial.send((uint8_t*)"AT+CIPSHUT\r\n",12);
}

void Cellular::SIM7000::TCPStartTask() {
	//AT+CSTT="CMNET" or vitamax.internet.vodafone.net (feltolto kartya eseten)
	char buffer[50] = "AT+CSTT=\"";
	strcat(buffer, params->getAPN());
	strcat(buffer, "\"\r\n");
	serial.send((uint8_t*)buffer,strlen(buffer));
}

void Cellular::SIM7000::TCPConnectToNetwork() {
	//AT+CIICR
	serial.send((uint8_t*)"AT+CIICR\r\n",10);
}

void Cellular::SIM7000::TCPOpenServer() {
	//AT+CIPSERVER=1,port
	char buffer[50] = "AT+CIPSERVER=1,";
	char port[10] = "";
	sprintf(port,"%d\r\n",params->getPort());
	strcat(buffer,port);
	serial.send((uint8_t*)buffer,strlen(buffer));
}

void Cellular::SIM7000::TCPCloseServer() {
	//AT+CIPSERVER=0
	serial.send((uint8_t*)"AT+CIPSERVER=0\r\n",16);
}

void Cellular::SIM7000::TCPDisConnectFromClient() {
	//AT+CIPCLOSE=client
	char buffer[50] ="AT+CIPCLOSE=";
	char client[5];
	sprintf(client,"%d\r\n",toSend->getClient());
	strcat(buffer,client);
	serial.send((uint8_t*)buffer,strlen(buffer));
}

//Bearer: int LTE it is a tunnel used to connect packet data networks (for example to connect internet)
//in elder architectures it was called PDP context (packet data protocol)
void Cellular::SIM7000::HTTPBearerConfigure() {
	//AT+SAPBR=3,1,"APN","CMNET"
	char buffer[100] = "AT+SAPBR=3,1,\"APN\",\"";
	strcat(buffer,params->getAPN());
	strcat(buffer,"\"\r\n");
	serial.send((uint8_t*)buffer,strlen(buffer));
}

void Cellular::SIM7000::HTTPBearerOpen() {
	//AT+SAPBR=1,1
	serial.send((uint8_t*)"AT+SAPBR=1,1\r\n",14);
}

void Cellular::SIM7000::HTTPBearerClose() {
	//AT+SAPBR=0,1
	serial.send((uint8_t*)"AT+SAPBR=0,1\r\n",14);
}

void Cellular::SIM7000::HTTPInit() {
	//AT+HTTPINIT
	serial.send((uint8_t*)"AT+HTTPINIT\r\n",13);
}

//CID=Content Interactive Delivery web service for providing rules to exchange documents
void Cellular::SIM7000::HTTPSetCID() {
	//AT+HTTPPARA="CID",1
	serial.send((uint8_t*)"AT+HTTPPARA=\"CID\",1\r\n",21);

}

void Cellular::SIM7000::HTTPSetURL() {
	//AT+HTTPPARA="URL","my url"
	char buffer[150] = "AT+HTTPPARA=\"URL\",";
	strcat(buffer,params->getURL());
	strcat(buffer,"\r\n");
	serial.send((uint8_t*)buffer,strlen(buffer));
}

void Cellular::SIM7000::HTTPStartGETSession() {
	//AT+HTTPACTION=0
	action = true;
	serial.send((uint8_t*)"AT+HTTPACTION=0\r\n",18);
}

void Cellular::SIM7000::HTTPSetPOSTData() {
	//AT+HTTPDATA=size,timeout
	char buffer[50] = "AT+HTTPDATA=";
	char size[10] = "";
	sprintf(size,"%d",toSend->getLength());
	strcat(buffer,size);
	strcat(buffer,",10000\r\n");
	serial.send((uint8_t*)buffer,strlen(buffer));
}

void Cellular::SIM7000::HTTPStartPOSTSession() {
	//AT+HTTPACTION=1
	action = true;
	serial.send((uint8_t*)"AT+HTTPACTION=1\r\n",18);
}

void Cellular::SIM7000::HTTPTerminate() {
	//AT+HTTPTERM
	serial.send((uint8_t*)"AT+HTTPTERM\r\n",13);
}
