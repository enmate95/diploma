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
	BLE,
}ESPProtocol_t;

typedef enum {
	WIFI_CLIENT,
	WIFI_SERVER,
	BLE_PERIPHERAL,
	BLE_CENTRAL,
}ESPRole_t;



class  ESPParams{
public:
		void setIP(char *IP) { memcpy(this->IP,IP,sizeof(this->IP));}
		void setSSID(char* SSID) { memcpy(this->SSID,SSID,sizeof(this->SSID));}
		void setPasskey(char* Passkey) { memcpy(this->Passkey,Passkey,sizeof(this->Passkey));}
		void setPort(int port) {this->port = port;}

		char* getIP() {return IP;}
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
				this->port = other.port;
			}
			return *this;
		}
private:
	char IP[50];
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
typedef std::function<void(char*,int)> ESPDataReceivedEventCb_t;
typedef std::function<void()> ESPConnectionClosedCb_t;
typedef std::function<void()> ESPConnectionOpenedCb_t;
typedef std::function<void(int)> ESPClientConnectedCb_t;
typedef std::function<void()> ESPTestCb_t;
typedef std::function<void()> ESPStartToSendCb_t;

typedef enum {
	ESP_DEVICE_PARAMS,
	ESP_MODE,
	ESP_ROLE,
}ESPParam_t;

typedef enum {
	ESP_GET_IP_CB,
	ESP_WIFI_CONNECTION_COMPLETE_CB,
	ESP_WIFI_DISCONNECTION_COMPLETE_CB,
	ESP_DATA_RECEIVED_CB,
	ESP_CONNECTION_OPENED_CB,
	ESP_CONNECTION_CLOSED_CB,
	ESP_CLIENT_CONNECTED_CB,
	ESP_TEST_CB,
}ESPCb_t;

typedef enum {
	ESP_DISABLE_ECHO,
	ESP_TEST,
	ESP_SET_MODE,
	ESP_CONNECT_TCP,
	ESP_CONNECT_WIFI,
	ESP_DISCONNECT_TCP,
	ESP_DISCONNECT_WIFI,
	ESP_GET_IP,
	ESP_DISABLE_MULTIPLE_CONN,
	ESP_ENABLE_MULTIPLE_CONN,
	ESP_OPEN_TCP_SERVER,
	ESP_CLOSE_TCP_SERVER,
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
	void Test();
	void SetMode();
	void ConnectTCP();
	void ConnectWifi();
	void DisConnectTCP();
	void DisConnectWifi();
	void GetIP();
	void DisableMultipleConnections();
	void EnableMultipleConnections();
	void OpenTCPServer();
	void CloseTCPServer();
	void DisableEcho();

	ESPGetIPCb_t ESPGetIPCallback;
	ESPWifiConnectionCompletedCb_t ESPWifiConnectionCompletedCallback;
	ESPWifiDisconnectionCompletedCb_t ESPWifiDisconnectionCompletedCallback;
	ESPDataReceivedEventCb_t ESPDataReceivedEventCallback;
	ESPConnectionClosedCb_t ESPConnectionClosedCallback;
	ESPConnectionOpenedCb_t ESPConnectionOpenedCallback;
	ESPClientConnectedCb_t ESPClientConnectedCallback;
	ESPTestCb_t ESPTestCallback;

	UartDMA &serial;
	UartData_t raw;
	ESPParams* params;
	ESPMode_t mode = ApAndStation;
	ESPRole_t role = WIFI_CLIENT;
	ESPProtocol_t protocol = TCP;
	ESPData* toSend;
	bool flag = false;
	bool test = false;
};

}


#endif /* ESP32_H_ */
