#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <utility>
#include "stm32f4xx_hal.h"

#define RAW_SIZE 250

typedef struct {
	uint32_t length = 0;
	uint8_t data[RAW_SIZE];
}UartData_t;

class UartDMA {
//constructors and destructors
public:
	UartDMA(UART_HandleTypeDef* handler);
	~UartDMA() = default;
//interfaces
public:
	bool send(uint8_t* data,size_t dataSize);
	void copyBuffer(UartData_t* copyTo);
	bool startDMA();
	void stopDMA();
	bool setLength();
	bool uartHandleFLags();
	void clearBuffer();
	UartData_t *getRaw();
//private variables
private:
	UartData_t rawBuff;
	UART_HandleTypeDef *handler = nullptr;
};

#endif
