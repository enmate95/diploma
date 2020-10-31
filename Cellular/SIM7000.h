#ifndef __SIM7000_H__
#define __SIM7000_H__

#include "uartDevice.h"
#include "RadioDevice.h"
#include <string.h>

namespace Cellular {

typedef enum {
	Automatic,
	LTE,
	GSM,
	GSM_LTE,
}SIM7000Mode_t;

typedef enum {
	TCP,
	HTTP,
	UDP,
 }SIM7000Role_t;

 typedef enum {
	 TCP_CLIENT,
	 TCP_SERVER,
	 UDP_NODE,
	 HTTP_GET,
	 HTTP_POST,
 }SIM7000Protocol_t;

 class  SIM7000Params{
 public:
 		void setIP(char *IP) { memcpy(this->IP,IP,sizeof(this->IP));}
 		void setPIN(int PIN) { this->PIN = PIN;}
 		void setAPN(char* APN) { memcpy(this->APN,APN,sizeof(this->APN));}
 		void setURL(char* URL) { memcpy(this->URL,URL,sizeof(this->URL));}
 		void setServerIP(char *ServerIP) { memcpy(this->ServerIP,ServerIP,sizeof(this->ServerIP));}
 		void setPort(int port) {this->port = port;}

 		char* getIP() {return IP;}
 		char* getAPN() {return APN;}
 		char* getURL() {return URL;}
 		int getPIN() {return PIN;}
 		char* getServerIP() {return ServerIP;}
 		int getPort() {return port;}

 		SIM7000Params() = default;
 		SIM7000Params(const SIM7000Params & other) { *this = other; }
 		SIM7000Params & operator=(const SIM7000Params & other) {
 			if(this != &other) {
 				memcpy(this->IP,other.IP,sizeof(this->IP));
 				memcpy(this->APN,other.APN,sizeof(this->APN));
 				memcpy(this->URL,other.URL,sizeof(this->URL));
 				memcpy(this->ServerIP,other.ServerIP,sizeof(this->ServerIP));
 				this->PIN = other.PIN;
 				this->port = other.port;
 			}
 			return *this;
 		}
 private:
 	char IP[50];
 	char ServerIP[50];
 	int port;
 	int PIN;
 	char APN[100];
 	char URL[100];
 };

 class SIM7000Data {
 public:
	 SIM7000Data(){}
	 SIM7000Data(const SIM7000Data & other) { *this = other; }
	 SIM7000Data & operator=(const SIM7000Data & other) {
 		if(this != &other) {
 			memcpy(this->raw,other.raw,sizeof(this->raw));
 		}
 		return *this;
 	}

 	void setData(char* data) {memcpy(raw,data,sizeof(raw));}
 	char* getData() {return raw;}

 	void setLength(int length) {this->length = length;}
 	int getLength() {return length;}
 private:
 	char raw[500] = "";
 	int length = 0;
 };

 typedef std::function<void(char*)> SIM7000GetIPCb_t;
 typedef std::function<void(char*,int)> SIM7000TCP_UDP_DataReceivedEventCb_t;
 typedef std::function<void()> SIM7000POSTSessionDoneCb_t;
 typedef std::function<void(char*,int)> SIM7000GETResultCb_t;

 typedef std::function<void()> SIM7000ConnectedToTCPServerCb_t;
 typedef std::function<void()> SIM7000DisconnectedFromTCPServerCb_t;

 typedef std::function<void()> SIM7000TCPServerOpenedCb_t;
 typedef std::function<void()> SIM7000TCPServerClosedCb_t;
 typedef std::function<void(int)> SIM7000TCPClientConnectedCb_t;
 typedef std::function<void(int)> SIM7000TCPClientDisconnectedCb_t;

 typedef std::function<void(bool)> SIM7000IsGPRSConnectedCb_t;


 typedef enum {
 	SIM7000_DEVICE_PARAMS,
	SIM7000_ROLE,
	SIM7000_PROTOCOL,
	SIM7000_CLIENT,
 }ESPParam_t;

 typedef enum {
 	SIM7000_GET_IP_CB,
	SIM7000_TCP_UDP_DATA_RECEIVED_CB,

	SIM7000_HTTP_GET_RECEIVED_CB,
	SIM7000_HTTP_POST_SENT_CB,

	SIM7000_TCP_CONNECTED_TO_SERVER_CB,
	SIM7000_TCP_DISCONNECTED_FROM_SERVER_CB,

	SIM7000_TCP_SERVER_OPENED_CB,
	SIM7000_TCP_SERVER_CLOSED_CB,
	SIM7000_TCP_CLIENT_CONNECTED_CB,
	SIM7000_TCP_CLIENT_DISCONNECTED_CB,

	SIM7000_GPRS_IS_CONNECTED_CB,
 }ESPCb_t;

 typedef enum {
	SIM7000_SET_PIN,
	SIM7000_SET_MODE,
	SIM7000_GET_IP,
	SIM7000_DISABLE_MULTIPLE_CONN,
	SIM7000_ENABLE_MULTIPLE_CONN,

	SIM7000_TCP_CONNECT_TO_SERVER,
	SIM7000_TCP_DISCONNECT_FROM_SERVER,

	SIM7000_TCP_OPEN_SERVER,
	SIM7000_TCP_CLOSE_SERVER,
	SIM7000_UDP_START,
	SIM7000_TCP_UDP_CONNECT_NETWORK,
	SIM7000_TCP_UDP_DISCONNECT_NETWORK,
	SIM7000_TCP_UDO_START_TASK,

	SIM7000_GET_GPRS_STATUS,
	SIM7000_HTTP_CONFIG_BEARER,
	SIM7000_HTTP_OPEN_BEARER,
	SIM7000_HTTP_CLOSE_BEARER,
	SIM7000_HTTP_INIT,
	SIM7000_HTTP_SET_CID,
	SIM7000_HTTP_SET_URL,
	SIM7000_HTTP_TERMINATE,
 }SIM7000Cmd_t;

class SIM7000 : public RadioDevice  {
public:
	 SIM7000(UartDMA & serial):serial(serial) {}
	~SIM7000()=default;

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
	void SetPIN();
	void GetGPRSState();
	void SetMode();
	void UDPStart();
	void ClearBuff();
	void GetIP();
	void EnableMultipleConnection();
	void DisableMultipleConnection();

	void TCPConnectServer();
	void TCPDisconnectFromServer();
	void TCPDisconnectFromNetwork();
	void TCPStartTask();
	void TCPConnectToNetwork();
	void TCPOpenServer();
	void TCPCloseServer();
	void TCPDisConnectFromClient();

	void HTTPBearerConfigure();
	void HTTPBearerOpen();
	void HTTPBearerClose();
	void HTTPInit();
	void HTTPSetCID();
	void HTTPSetURL();
	void HTTPStartGETSession();
	void HTTPSetPOSTData();
	void HTTPStartPOSTSession();
	void HTTPTerminate();

	void HandleStatusOK();
	void HandleStatusERROR();
	void HandleClientDisconnection(char *ptr);
	void HandleClientConnection(char *ptr);
	void HandleServerDisconnection();
	void HandleDataReception(char* ptr);
	void HandleHTTPPostDone();

	bool IsNumber(char c);
	void ProcessDone();
	void HandleHTTP();
	void HandleTCP_UDP();

	SIM7000GetIPCb_t SIM7000GetIPCallback;
	SIM7000TCP_UDP_DataReceivedEventCb_t SIM7000TCP_UDP_DataReceivedEventCallback;
	SIM7000GETResultCb_t SIM7000GETResultCallback;
	SIM7000POSTSessionDoneCb_t SIM7000POSTSessionDoneCallback;

	SIM7000ConnectedToTCPServerCb_t SIM7000ConnectedToTCPServerCallback;
	SIM7000DisconnectedFromTCPServerCb_t SIM7000DisconnectedFromTCPServerCallback;

	SIM7000TCPServerOpenedCb_t SIM7000TCPServerOpenedCallback;
	SIM7000TCPServerClosedCb_t SIM7000TCPServerClosedCallback;
	SIM7000TCPClientConnectedCb_t SIM7000TCPClientConnectedCallback;
	SIM7000TCPClientDisconnectedCb_t SIM7000TCPClientDisconnectedCallback;
	SIM7000IsGPRSConnectedCb_t SIM7000IsGPRSConnectedCallback;

private:
   UartDMA & serial;
   UartData_t raw;
   SIM7000Params *params;
   SIM7000Mode_t mode = Automatic;
   SIM7000Role_t role = HTTP;
   SIM7000Protocol_t protocol = HTTP_GET;
   SIM7000Data *toSend;
   int currentClient = 0;
   bool flag = false;
   bool action = false;
 };

}

#endif
