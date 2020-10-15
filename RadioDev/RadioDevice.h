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

class RadioDevice {
	public:
		virtual ~RadioDevice() {}

		virtual void init(const Container & data) = 0;

		virtual void setParameter(int param, const Container & value) = 0;
		
		virtual Container getParameter(int param) = 0;

		virtual void setCallback(int type, const Container & value) = 0;

		virtual void process() = 0;

		bool isBusy() { return busy;}

		virtual bool send(const Container & data) = 0;

		virtual bool sendCmd(int cmd) = 0;

		virtual void start() = 0;

		virtual void stop() = 0;

		virtual void callHandler() = 0;

	protected:
		bool busy = false;
};


#endif /* RADIODEVICE_H_ */
