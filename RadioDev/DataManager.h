/*
 * DataManager.h
 *
 *  Created on: 4 Oct 2020
 *      Author: horny
 */

#ifndef DATAMANAGER_H_
#define DATAMANAGER_H_

#include "Container.h"

template <typename TYPE>
class DataManager {

public:
	static Container & convertFromData(TYPE data) {
		static Container container;
		static Container::containerDerived<TYPE> derived;
		container.ptr = &derived;
		derived.set(data);
		return container;
	}
	static TYPE convertFromContainer(Container con) {return static_cast<Container::containerDerived<TYPE>*>(con.ptr)->get();}
};


#endif /* DATAMANAGER_H_ */
