/*
 * SLIP.cpp
 *
 *  Created on: Mar 25, 2020
 *      Author: mate_pc
 */

 /******************************************************************************
 Copyright (c) 2009 IMST GmbH.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are NOT permitted without prior written permission
 of the IMST GmbH.

 THIS SOFTWARE IS PROVIDED BY THE IMST GMBH AND CONTRIBUTORS ``AS IS'' AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE IMST GMBH OR CONTRIBUTORS BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 THIS SOFTWARE IS CLASSIFIED AS: CONFIDENTIAL

 *******************************************************************************/
#include "SLIP.h"

// SLIP Protocol Characters
#define SLIP_END					0xC0
#define	SLIP_ESC					0xDB
#define	SLIP_ESC_END				0xDC
#define	SLIP_ESC_ESC				0xDD

// SLIP Receiver/Decoder States
#define SLIPDEC_IDLE_STATE          0
#define	SLIPDEC_START_STATE			1
#define	SLIPDEC_IN_FRAME_STATE		2
#define	SLIPDEC_ESC_STATE			3


#define NUMBER_OF_WAKEUP_CHARS      40



typedef struct {
    // upper layer client
    TComSlipCbByteIndication    RxIndication;

    // receiver/decoder state
    int                         RxState;

    // rx buffer index for next decodedrxByte
    uint16_t                      RxIndex;

    // size of RxBuffer
    uint16_t                      RxBufferSize;

    // pointer to RxBuffer
    uint8_t*                      RxBuffer;
}TComSlip;


TComSlip ComSlip;
UART_HandleTypeDef *uart_handler = nullptr;

//------------------------------------------------------------------------------
//
//  ComSlip_Init
//
//------------------------------------------------------------------------------
void ComSlip_Init(UART_HandleTypeDef *huart)
{
    // init to idle state, no rx-buffer avaliable
    ComSlip.RxState         =   SLIPDEC_IDLE_STATE;
    ComSlip.RxIndex         =   0;
    ComSlip.RxBuffer        =   0;
    ComSlip.RxBufferSize    =   0;
    uart_handler = huart;
}

void ComSlip_RegisterClient(TComSlipCbByteIndication cbRxIndication) {
    ComSlip.RxIndication = cbRxIndication;
}

void ComSlip_StoreRxByte(uint8_t rxByte) {
    if (ComSlip.RxIndex < ComSlip.RxBufferSize)
        ComSlip.RxBuffer[ComSlip.RxIndex++] = rxByte;
}

bool ComSlip_ProcessRxMsg() {
    // forward received message to upper layer and allocate new rx-buffer
    if(ComSlip.RxIndication) {
        (*ComSlip.RxIndication)(ComSlip.RxBuffer, ComSlip.RxIndex);
    }
    // new buffer avaliable ?
    if (ComSlip.RxBuffer != 0) {
        // yes, keep receiver enabled
        return true;
    }
    // no further rx-buffer, stop receiver/decoder
    return false;
}

bool ComSlip_SendMessage(uint8_t* msg, uint16_t msgLength) {
    int countWakeupChars = NUMBER_OF_WAKEUP_CHARS;
    uint8_t tx[2] = {0,0};
    // send start of SLIP message
    // send wakeup chars
    while (countWakeupChars--) {
    	tx[0] = SLIP_END;
        HAL_UART_Transmit(uart_handler,&tx[0],1,100);
    }

    // iterate over all message bytes
    while(msgLength--) {
        switch (*msg) {
            case SLIP_END:
            	tx[0] = SLIP_ESC;
            	tx[1] = SLIP_ESC_END;
            	 HAL_UART_Transmit(uart_handler,tx,2,100);
                break;

            case SLIP_ESC:
            	tx[0] = SLIP_ESC;
				tx[1] = SLIP_ESC_ESC;
				HAL_UART_Transmit(uart_handler,tx,2,100);
                break;

            default:
            	HAL_UART_Transmit(uart_handler,msg,1,100);
                break;
        }
        // next byte
        msg++;
    }

    // send end of SLIP message
    tx[0] = SLIP_END;
    HAL_UART_Transmit(uart_handler,&tx[0],1,100);
    // always ok
    return true;
}

bool ComSlip_SetRxBuffer(uint8_t* rxBuffer, uint16_t  rxBufferSize) {
    // receiver in IDLE state and client already registered ?
    if((ComSlip.RxState == SLIPDEC_IDLE_STATE) && ComSlip.RxIndication) {
        // same buffer params
        ComSlip.RxBuffer        = rxBuffer;
        ComSlip.RxBufferSize    = rxBufferSize;

        // enable decoder
        ComSlip.RxState = SLIPDEC_START_STATE;

        return true;
    }
    return false;
}

void ComSlip_ProcessRxByte(uint8_t rxData) {
    // get rxByte
    uint8_t rxByte = rxData;

    // decode according to current state
    switch(ComSlip.RxState) {
        case    SLIPDEC_START_STATE:
                // start of SLIP frame ?
                if(rxByte == SLIP_END) {
                    // init read index
                    ComSlip.RxIndex = 0;

                    // next state
                    ComSlip.RxState = SLIPDEC_IN_FRAME_STATE;
                }
                break;

        case    SLIPDEC_IN_FRAME_STATE:
                switch(rxByte) {
                    case    SLIP_END:
                            // data received ?
                            if(ComSlip.RxIndex > 0) {
                                // yes, process rx message, get new buffer
                                if (ComSlip_ProcessRxMsg()) {
                                    ComSlip.RxState = SLIPDEC_START_STATE;
                                }
                                else {
                                    // disable decoder, temp. no buffer avaliable
                                    ComSlip.RxState = SLIPDEC_IDLE_STATE;
                                }
                            }
                            // init read index
                            ComSlip.RxIndex = 0;
                            break;

                    case  SLIP_ESC:
                            // enter escape sequence state
                            ComSlip.RxState = SLIPDEC_ESC_STATE;
                            break;

                    default:
                            // store byte
                            ComSlip_StoreRxByte(rxByte);
                            break;
                }
                break;

        case    SLIPDEC_ESC_STATE:
                switch(rxByte) {
                    case    SLIP_ESC_END:
                            ComSlip_StoreRxByte(SLIP_END);
                            // quit escape sequence state
                            ComSlip.RxState = SLIPDEC_IN_FRAME_STATE;
                            break;

                    case    SLIP_ESC_ESC:
                            ComSlip_StoreRxByte(SLIP_ESC);
                            // quit escape sequence state
                            ComSlip.RxState = SLIPDEC_IN_FRAME_STATE;
                            break;

                    default:
                            // abort frame receiption
                            ComSlip.RxState = SLIPDEC_START_STATE;
                            break;
                }
                break;

        default:
                break;
    }
}


