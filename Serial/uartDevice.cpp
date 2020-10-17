#include "uartDevice.h"
#include <string.h>

UartDMA::UartDMA(UART_HandleTypeDef* handler): handler(handler) {
	clearBuffer();
}

bool UartDMA::send(uint8_t* data,size_t dataSize) {
	HAL_StatusTypeDef ret = HAL_UART_Transmit(handler, data, static_cast<uint16_t>(dataSize),10);
	return (ret == HAL_OK);
}


bool UartDMA::uartHandleFLags() {
	uint32_t flags = handler->Instance->SR;
	if((flags & UART_FLAG_IDLE)) {
		__HAL_UART_CLEAR_IDLEFLAG(handler);
		return true;
	}
	return false;
}

UartData_t* UartDMA::getRaw() { return &rawBuff;}

void UartDMA::clearBuffer() {
	for(uint32_t i = 0; i < rawBuff.length; i++) {
		rawBuff.data[i] = 0;
	}
	rawBuff.length = 0;
}

void UartDMA::copyBuffer(UartData_t* copyTo) {
	HAL_UART_DMAPause(handler);
	memcpy(copyTo->data,rawBuff.data,rawBuff.length);
	copyTo->length = rawBuff.length;
	HAL_UART_DMAResume(handler);
}

bool UartDMA::startDMA() {
	__HAL_UART_ENABLE_IT(handler, UART_IT_IDLE);
	if (HAL_UART_Receive_DMA(handler, rawBuff.data, RAW_SIZE) != HAL_OK) {
		return false;
	}
	return true;
}

void UartDMA::stopDMA() {
	HAL_UART_DMAStop(handler);
}

bool UartDMA::setLength() {
	HAL_UART_DMAPause(handler);
	rawBuff.length = (uint32_t)RAW_SIZE - handler->hdmarx->Instance->NDTR;
	HAL_UART_DMAResume(handler);
	if (rawBuff.length > 0) {
		return true;
	}
	return false;
}
