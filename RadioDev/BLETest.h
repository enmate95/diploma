/*
 * BLETest.h
 *
 *  Created on: 4 Oct 2020
 *      Author: horny
 */

#ifndef BLETEST_H_
#define BLETEST_H_
#include "Container.h"
#include "RadioDevice.h"
#include <string.h>
#include <stdint.h>

class BLERadioConfigData {
public:
	void seta(int a) {this->a = a;}
	void setb(int b) {this->b = b;}
	int geta() {return a;}
	int getb() {return b;}
	BLERadioConfigData() {}
	BLERadioConfigData(const BLERadioConfigData & other) { *this = other;}
	BLERadioConfigData& operator=(const BLERadioConfigData & other) {
		a = other.a;
		b = other.b;
		return *this;
	}
private:
	int a;
	int b;
};

class BLEData {
public:
	BLEData() {}
	BLEData(const BLEData & other) { *this = other;}
	BLEData& operator=(const BLEData & other) {
		if(this != &other) {
			memcpy(raw,other.raw,other.length);
			length = other.length;
		}

		return *this;
	}
	uint8_t raw[10];
	uint16_t length;
};

class BLECommands {
public:
	static const int startAdvertising = 0;
	static const int setPasskey = 1;
};

class Params {
public:
	void set(int param, Container val){
		this->param = param;
		this->val = val;
	}
	int getParam() {return param;}
	Container getVal() {return val;}
protected:
	int param;
	Container val;
};

class BLEParams : public Params {
public:
	static const int advName = 1;
	static const int passkey = 2;
};


class BLEDevice: public RadioDevice {
public:
	BLEDevice(){}
	~BLEDevice(){}
	void init(const Container & data){}


	//while(validData) {callFunc()}
	void process() {}

	bool isBusy() {return true;}

	bool send(const Container & data) {
		this->data = static_cast<Container::containerDerived<BLEData>*>(data.ptr)->get();
		//ble.send(data);
		return true;
	}

	bool sendCmd(int cmd) {
		switch(cmd) {
		case BLECommands::setPasskey:
				//call ble private function
				//before this call member variable passkey was set...
			break;
		}
		return true;
	}

	void setParameter(int param, const Container & value) {
		switch(param) {
		case BLEParams::passkey:
				DataManager<uint32_t> key;
				passkey = key.convertFromContainer(value);
			break;
		}
	}

	Container getParameter(int param) {
		DataManager<uint32_t> key;
		return key.convertFromData(passkey);
	}

	void setCallback(int type, const Container & value) {}

	void test(){}
private:
	BLERadioConfigData conf;
	BLEData data;
	uint32_t passkey;
};




#endif /* BLETEST_H_ */
