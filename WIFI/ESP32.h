/*
 * ESP32.h
 *
 *  Created on: 11 Oct 2020
 *      Author: horny
 */

#ifndef ESP32_H_
#define ESP32_H_

#include "uartDevice.h"
#include "RadioDevice.h"
#include <string.h>

namespace WifiDevice {

typedef enum {
	AccesPoint,
	Station,
	ApAndStation
} ESPMode_t;

typedef enum {
	TCP,
	UDP,
}ESPProtocol_t;

typedef enum {
	TCP_CLIENT,
	TCP_SERVER,
}ESPRole_t;



class  ESPParams{
public:
		void setIP(char *IP) { memcpy(this->IP,IP,sizeof(this->IP));}
		void setServerIP(char *ServerIP) { memcpy(this->ServerIP,ServerIP,sizeof(this->ServerIP));}
		void setSSID(char* SSID) { memcpy(this->SSID,SSID,sizeof(this->SSID));}
		void setPasskey(char* Passkey) { memcpy(this->Passkey,Passkey,sizeof(this->Passkey));}
		void setPort(int port) {this->port = port;}

		char* getIP() {return IP;}
		char* getServerIP() {return ServerIP;}
		char* getSSID() {return SSID;}
		char* getPasskey() {return Passkey;}
		int getPort() {return port;}

		ESPParams() = default;
		ESPParams(const ESPParams & other) { *this = other; }
		ESPParams & operator=(const ESPParams & other) {
			if(this != &other) {
				memcpy(this->IP,other.IP,sizeof(this->IP));
				memcpy(this->SSID,other.SSID,sizeof(this->SSID));
				memcpy(this->Passkey,other.Passkey,sizeof(this->Passkey));
				memcpy(this->ServerIP,other.ServerIP,sizeof(this->ServerIP));
				this->port = other.port;
			}
			return *this;
		}
private:
	char IP[50];
	char ServerIP[50];
	char SSID[100];
	char Passkey[50];
	int port;
};

class ESPData {
public:
	ESPData(){}
	ESPData(const ESPData & other) { *this = other; }
	ESPData & operator=(const ESPData & other) {
		if(this != &other) {
			memcpy(this->raw,other.raw,sizeof(this->raw));
			this->client = other.client;
		}
		return *this;
	}

	void setData(char* data) {memcpy(raw,data,sizeof(raw));}
	char* getData() {return raw;}
	void setClinet(int client) {this->client = client;}
	int getClient() {return client;}
	void setLength(int length) {this->length = length;}
	int getLength() {return length;}
private:
	char raw[200] = "";
	int length = 0;
	int client = 0;
};

typedef std::function<void(char*)> ESPGetIPCb_t;

typedef std::function<void()> ESPWifiConnectionCompletedCb_t;
typedef std::function<void()>ESPWifiDisconnectionCompletedCb_t;

typedef std::function<void(char*,int,int)> ESPDataReceivedEventCb_t;

typedef std::function<void()> ESPConnectedToTCPServerCb_t;
typedef std::function<void()> ESPDisconnectedFromTCPServerCb_t;

typedef std::function<void()> ESPTCPServerOpenedCb_t;
typedef std::function<void()> ESPTCPServerClosedCb_t;
typedef std::function<void(int)> ESPTCPClientConnectedCb_t;
typedef std::function<void(int)> ESPTCPClientDisonnectedCb_t;

typedef std::function<void(bool)> ESPWifiisConnectedCb_t;
typedef std::function<void(bool)> ESPIsStatusOKCb_t;

enum class ESPCommandState {
	IDLE,
	SET_MODE,
	START_UDP,
	ENABLE_MULTIPLE_CONN,
	DISABLE_MULTIPLE_CONN,
	DISABLE_ECHO,
	GET_IP,
	GET_WIFI_STATUS,
	SEND,

	CONNECT_WIFI,
	DISCONNECT_WIFI,

	CONNECT_TO_TCP_SERVER,
	DISCONNECT_FROM_TCP_SERVER,

	OPEN_TCP_SERVER,
	CLOSE_TCP_SERVER,
	DISCONNECT_FROM_TCP_CLIENT,
};

typedef enum {
	ESP_DEVICE_PARAMS,
	ESP_ROLE,
	ESP_PROTOCOL,
}ESPParam_t;

typedef enum {
	ESP_GET_IP_CB,
	ESP_STATUS_OK_CB,

	ESP_WIFI_CONNECTION_COMPLETE_CB,
	ESP_WIFI_DISCONNECTION_COMPLETE_CB,

	ESP_DATA_RECEIVED_CB,

	ESP_CONNECTED_TO_TCP_SERVER_CB,
	ESP_DISCONNECTED_FROM_TCP_SERVER_CB,

	ESP_SERVER_OPENED_CB,
	ESP_SERVER_CLOSED_CB,
	ESP_TCP_CLIENT_CONNECTED_CB,
	ESP_TCP_CLIENT_DISCONNECTED_CB,

	ESP_WIFI_IS_CONNECTED_CB,
}ESPCb_t;

typedef enum {
	ESP_SET_MODE,
	ESP_DISABLE_ECHO,

	ESP_CONNECT_WIFI,
	ESP_DISCONNECT_WIFI,

	ESP_CONNECT_TO_TCP_SERVER,
	ESP_DISCONNECT_FROM_TCP_SERVER,

	ESP_OPEN_TCP_SERVER,
	ESP_CLOSE_TCP_SERVER,
	ESP_DISCONNECT_FROM_TCP_CLIENT,

	ESP_GET_IP,

	ESP_DISABLE_MULTIPLE_CONN,
	ESP_ENABLE_MULTIPLE_CONN,

	ESP_START_UDP,

	ESP_GET_WIFI_STATUS,
}ESPCmd_t;

class ESP32: public RadioDevice{
public:
	ESP32(UartDMA & serial):serial(serial) {}
	~ESP32()=default;

	void init(const Container & data);
	void setParameter(int param, const Container & value);
	Container getParameter(int param);
	void setCallback(int type, const Container & value);
	void process();
	bool send(const Container & data);
	bool sendCmd(int cmd);
	void callHandler();
	void start();
	void stop();

private:
	void SetMode();
	void ConnectToTCPServer();
	void ConnectWifi();
	void DisConnectWifi();
	void DisConnectFromTCPServer();
	void DisConnectFromTCPClient();
	void OpenTCPServer();
	void CloseTCPServer();
	void StartUDP();

	void GetIP();
	void DisableMultipleConnections();
	void EnableMultipleConnections();
	void GetWifiConnectionStatus();
	void ClearBuff();
	void DisableEcho();

	void HandleTCPClient();
	void HandleTCPServer();
	void HandleUDP();

	bool IsNumber(char c);
	void ProcessDone();

	ESPGetIPCb_t ESPGetIPCallback;

	ESPWifiConnectionCompletedCb_t ESPWifiConnectionCompletedCallback;
	ESPWifiDisconnectionCompletedCb_t ESPWifiDisconnectionCompletedCallback;

	ESPDataReceivedEventCb_t ESPDataReceivedEventCallback;

	ESPConnectedToTCPServerCb_t ESPConnectedToTCPServerCallback;
	ESPDisconnectedFromTCPServerCb_t ESPDisconnectedFromTCPServerCallback;

	ESPTCPServerOpenedCb_t ESPTCPServerOpenedCallback;
	ESPTCPServerClosedCb_t ESPTCPServerClosedCallback;
	ESPTCPClientConnectedCb_t ESPTCPClientConnectedCallback;
	ESPTCPClientDisonnectedCb_t ESPTCPClientDisonnectedCallback;

	ESPIsStatusOKCb_t ESPIsStatusOKCallback;
	ESPWifiisConnectedCb_t ESPWifiisConnectedCallback;

	UartDMA &serial;
	UartData_t raw;
	ESPParams* params;
	ESPMode_t mode = ApAndStation;
	ESPRole_t role = TCP_CLIENT;
	ESPProtocol_t protocol = TCP;
	ESPData* toSend;
	ESPCommandState state = ESPCommandState::IDLE;
	bool flag = false;
};

}


#endif /* ESP32_H_ */
