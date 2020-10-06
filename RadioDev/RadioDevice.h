/*
 * RadioDdevice.h
 *
 *  Created on: 3 Oct 2020
 *      Author: horny
 */

#ifndef RADIODEVICE_H_
#define RADIODEVICE_H_

#include "Container.h"
#include <functional>

typedef std::function<void()> ITHandlerCb_t;

class RadioDevice {
	public:
		virtual ~RadioDevice() {}

		virtual void init(const Container & data) = 0;

		virtual void setParameter(int param, const Container & value) = 0;
		
		virtual Container getParameter(int param) = 0;

		virtual void setCallback(int type, const Container & value) = 0;

		//while(validData) {callFunc()}
		virtual void process() = 0;

		bool isBusy() { return busy;}

		virtual bool send(const Container & data) = 0;

		virtual bool sendCmd(int cmd) = 0;
	protected:
		bool busy = false;
		bool event = false;
};


#endif /* RADIODEVICE_H_ */
