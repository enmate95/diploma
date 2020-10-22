#include "SIM7000.h"
#include "DataManager.h"

void Cellular::SIM7000::init(const Container & data) {
	mode = DataManager<Cellular::SIM7000Mode_t>::convertFromContainer(data);
}

void Cellular::SIM7000::setParameter(int param, const Container & value) {
	switch(param) {
		case SIM7000_DEVICE_PARAMS: {
			params = &DataManager<Cellular::SIM7000Params>::convertFromContainer(value);
		}
		break;

		case SIM7000_ROLE: {
			role = DataManager<Cellular::SIM7000Role_t>::convertFromContainer(value);
		}
		break;

		case SIM7000_PROTOCOL: {
			protocol = DataManager<Cellular::SIM7000Protocol_t>::convertFromContainer(value);
		}
		break;
	}
}

Container Cellular::SIM7000::getParameter(int param) {
	switch(param) {
		case SIM7000_DEVICE_PARAMS: {
			return DataManager<Cellular::SIM7000Params>::convertFromData(*params);
		}
		break;

		case SIM7000_ROLE: {
			return DataManager<Cellular::SIM7000Role_t>::convertFromData(role);
		}
		break;

		case SIM7000_PROTOCOL: {
			return DataManager<Cellular::SIM7000Protocol_t>::convertFromData(protocol);
		}
		break;
	}
}

void Cellular::SIM7000::setCallback(int type, const Container & value) {
	switch(type) {
		case SIM7000_GET_IP_CB: {
			SIM7000GetIPCallback = DataManager<Cellular::SIM7000GetIPCb_t>::convertFromContainer(value);
		}
		break;

		case SIM7000_TCP_DATA_RECEIVED_CB: {
			SIM7000TCPDataReceivedEventCallback = DataManager<Cellular::SIM7000TCPDataReceivedEventCb_t>::convertFromContainer(value);
		}
		break;

		case SIM7000_GET_RECEIVED_CB: {
			SIM7000GETResultCallback = DataManager<Cellular::SIM7000GETResultCb_t>::convertFromContainer(value);
		}
		break;

		case SIM7000_POST_SENT_CB: {
			SIM7000POSTSessionDoneCallback = DataManager<Cellular::SIM7000POSTSessionDoneCb_t>::convertFromContainer(value);
		}
		break;

		case SIM7000_CONNECTED_TO_TCP_SERVER_CB: {
			SIM7000ConnectedToTCPServerCallback = DataManager<Cellular::SIM7000ConnectedToTCPServerCb_t>::convertFromContainer(value);
		}
		break;

		case SIM7000_DISCONNECTED_FROM_TCP_SERVER_CB: {
			SIM7000DisconnectedFromTCPServerCallback = DataManager<Cellular::SIM7000DisconnectedFromTCPServerCb_t>::convertFromContainer(value);
		}
		break;

		case SIM7000_SERVER_OPENED_CB: {
			SIM7000TCPServerOpenedCallback = DataManager<Cellular::SIM7000TCPServerOpenedCb_t>::convertFromContainer(value);
		}
		break;

		case SIM7000_SERVER_CLOSED_CB: {
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

		case SIM7000_TEST_CB: {
			SIM7000TestCallback = DataManager<Cellular::SIM7000TestCb_t>::convertFromContainer(value);
		}
		break;

		case SIM7000_GPRS_IS_CONNECTED_CB: {
			SIM7000IsGPRSConnectedCallback = DataManager<Cellular::SIM7000IsGPRSConnectedCb_t>::convertFromContainer(value);
		}
		break;
		}
}

void Cellular::SIM7000::process() {

}

bool Cellular::SIM7000::send(const Container & data) {

}

bool Cellular::SIM7000::sendCmd(int cmd) {

}

void Cellular::SIM7000::callHandler() {

}

void Cellular::SIM7000::start() {

}

void Cellular::SIM7000::stop() {

}

bool Cellular::SIM7000::isStatusOk() {

}

void Cellular::SIM7000::GetIP() {
	//AT+CIFSR
	serial.send((uint8_t*)"AT+CIFSR\r\n",10);
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
	sprintf(client,"%d\r\n",currentClient);
	strcat(buffer,client);
	serial.send((uint8_t*)buffer,strlen(buffer));
}

//Bearer: int LTE it is a tunnel used to connect packet data networks (for example to connect internet)
//in elder architectures it was called PDP context (packet data protocol)
void Cellular::SIM7000::HTTPBearerConfigure() {
	//AT+SAPBR=3,1,"APN","CMNET"
	char buffer[50] = "AT+SAPBR=3,1,\"APN\",\"";
	strcat(buffer,params->getAPN());
	strcat(buffer,"\r\n");
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
	serial.send((uint8_t*)"AT+HTTPACTION=0\r\n",18);
}

void Cellular::SIM7000::HTTPSetPOSTData() {
	//AT+HTTPDATA=size,timeout
	char buffer[50] = "AT+HTTPDATA=";
	char size[10] = "";
	sprintf(size,"%d",toSend->getLength());
	strcat(buffer,",10000");
	serial.send((uint8_t*)buffer,strlen(buffer));
}

void Cellular::SIM7000::HTTPStartPOSTSession() {
	//AT+HTTPACTION=1
	serial.send((uint8_t*)"AT+HTTPACTION=1\r\n",18);
}

void Cellular::SIM7000::HTTPTerminate() {
	//AT+HTTPTERM
	serial.send((uint8_t*)"AT+HTTPTERM\r\n",13);
}
