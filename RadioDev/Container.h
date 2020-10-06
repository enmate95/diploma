/*
 * Container.h
 *
 *  Created on: 3 Oct 2020
 *      Author: horny
 */

#ifndef CONTAINER_H_
#define CONTAINER_H_

class Container {
public:
	Container(){}
	Container(const Container & other) { *this = other; }

	Container& operator=(const Container & other) {
		ptr = other.ptr;
		return *this;
	}

	class containerBase {};
	template <typename T>
	class containerDerived : public containerBase {
	public:
		T data;
		void set(T newData) {data = newData;}
		T get() {return data;}
	};
	containerBase* ptr = nullptr;
};


#endif /* CONTAINER_H_ */
