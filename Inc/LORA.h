/*
 * LORA.h
 *
 *  Created on: Mar 25, 2020
 *      Author: mate_pc
 */

 /******************************************************************************
 Copyright (c) 2015 IMST GmbH.
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


#ifndef LORA_H_
#define LORA_H_

#include "stm32f4xx_hal.h"

#define DEVICE_ADDR_LEN      4
#define KEY_LEN             16
#define EUI_LEN              8

typedef uint16_t TWiMODLRResult;


typedef enum {
    WiMODLR_RESULT_OK = 0,
    WiMODLR_RESULT_PAYLOAD_LENGTH_ERROR,
    WiMODLR_RESULT_PAYLOAD_PTR_ERROR,
    WiMODLR_RESULT_TRANMIT_ERROR
}TWiMDLRResultcodes;

#define DEVMGMT_ID                          0x01
#define LORAWAN_ID                          0x10

// Status Codes
#define	DEVMGMT_STATUS_OK					0x00
#define	DEVMGMT_STATUS_ERROR				0x01
#define	DEVMGMT_STATUS_CMD_NOT_SUPPORTED	0x02
#define	DEVMGMT_STATUS_WRONG_PARAMETER		0x03
#define DEVMGMT_STATUS_WRONG_DEVICE_MODE    0x04

// Message IDs
#define DEVMGMT_MSG_PING_REQ                0x01
#define DEVMGMT_MSG_PING_RSP                0x02

#define DEVMGMT_MSG_GET_DEVICEINFO_REQ      0x03
#define DEVMGMT_MSG_GET_DEVICEINFO_RSP      0x04

#define DEVMGMT_MSG_GET_FW_VERSION_REQ      0x05
#define DEVMGMT_MSG_GET_FW_VERSION_RSP      0x06

#define DEVMGMT_MSG_RESET_REQ                   0x07
#define DEVMGMT_MSG_RESET_RSP                   0x08

#define DEVMGMT_MSG_SET_OPMODE_REQ              0x09
#define DEVMGMT_MSG_SET_OPMODE_RSP              0x0A
#define DEVMGMT_MSG_GET_OPMODE_REQ              0x0B
#define DEVMGMT_MSG_GET_OPMODE_RSP              0x0C

// Status Codes
#define LORAWAN_STATUS_OK	                0x00	// Operation successful
#define LORAWAN_STATUS_ERROR	            0x01	// Operation failed
#define LORAWAN_STATUS_CMD_NOT_SUPPORTED	0x02	// Command is not supported
#define LORAWAN_STATUS_WRONG_PARAMETER	    0x03	// HCI message contains wrong parameter
#define LORAWAN_STATUS_DEVICE_NOT_ACTIVATED	0x05	// Device is not activated
#define LORAWAN_STATUS_DEVICE_BUSY	        0x06	// Device is busy
#define LORAWAN_STATUS_LENGTH_ERROR	        0x08	// Radio packet length invalid

// Message IDs
#define LORAWAN_MSG_ACTIVATE_DEVICE_REQ	    0x01
#define LORAWAN_MSG_ACTIVATE_DEVICE_RSP	    0x02
#define LORAWAN_MSG_SET_JOIN_PARAM_REQ	    0x05
#define LORAWAN_MSG_SET_JOIN_PARAM_RSP	    0x06
#define LORAWAN_MSG_JOIN_NETWORK_REQ	    0x09
#define LORAWAN_MSG_JOIN_NETWORK_RSP	    0x0A
#define LORAWAN_MSG_JOIN_NETWORK_TX_IND	    0x0B
#define LORAWAN_MSG_JOIN_NETWORK_IND	    0x0C
#define LORAWAN_MSG_SEND_UDATA_REQ	        0x0D
#define LORAWAN_MSG_SEND_UDATA_RSP	        0x0E
#define LORAWAN_MSG_SEND_UDATA_TX_IND	    0x0F
#define LORAWAN_MSG_RECV_UDATA_IND	        0x10
#define LORAWAN_MSG_SEND_CDATA_REQ	        0x11
#define LORAWAN_MSG_SEND_CDATA_RSP	        0x12
#define LORAWAN_MSG_SEND_CDATA_TX_IND	    0x13
#define LORAWAN_MSG_RECV_CDATA_IND	        0x14
#define LORAWAN_MSG_RECV_ACK_IND	        0x15
#define LORAWAN_MSG_SET_RSTACK_CONFIG_REQ   0x19
#define LORAWAN_MSG_SET_RSTACK_CONFIG_RSP   0x1A
#define LORAWAN_MSG_GET_RSTACK_CONFIG_REQ   0x1B
#define LORAWAN_MSG_GET_RSTACK_CONFIG_RSP   0x1C
#define LORAWAN_MSG_REACTIVATE_DEVICE_REQ   0x1D
#define LORAWAN_MSG_REACTIVATE_DEVICE_RSP   0x1E
#define LORAWAN_MSG_DEACTIVATE_DEVICE_REQ   0x21
#define LORAWAN_MSG_DEACTIVATE_DEVICE_RSP   0x22
#define LORAWAN_MSG_FACTORY_RESET_REQ           0x23
#define LORAWAN_MSG_FACTORY_RESET_RSP           0x24
#define LORAWAN_MSG_SET_DEVICE_EUI_REQ          0x25
#define LORAWAN_MSG_SET_DEVICE_EUI_RSP          0x26

//System Operation Modes
#define LORAWAN_OPMODE_STANDARD                 0x00
#define LORAWAN_OPMODE_CUSTOMER_MODE            0x03

// message header size: 2 bytes for SapID + MsgID
#define WIMODLR_HCI_MSG_HEADER_SIZE	 	2

// message payload size
#define WIMODLR_HCI_MSG_PAYLOAD_SIZE	300

// frame check sequence field size: 2 bytes for CRC16
#define WIMODLR_HCI_MSG_FCS_SIZE		2


#define LORA_MAC_PORT                   0x02


// visible max. buffer size for lower SLIP layer
#define WIMODLR_HCI_RX_MESSAGE_SIZE (WIMODLR_HCI_MSG_HEADER_SIZE\
                                    +WIMODLR_HCI_MSG_PAYLOAD_SIZE\
                                    +WIMODLR_HCI_MSG_FCS_SIZE)


typedef struct {
    // Payload Length Information, not transmitted over UART interface !
    uint16_t  Length;

    // Service Access Point Identifier
    uint8_t   SapID;

    // Message Identifier
    uint8_t   MsgID;

    // Payload Field
    uint8_t   Payload[WIMODLR_HCI_MSG_PAYLOAD_SIZE];

    // Frame Check Sequence Field
    uint8_t   CRC16[WIMODLR_HCI_MSG_FCS_SIZE];

}TWiMODLR_HCIMessage;

//------------------------------------------------------------------------------
//  Common Transceicer States & Flags
//------------------------------------------------------------------------------

typedef enum
{
    trx_RXDone,
    trx_TXDone,
    trx_ACKDone,
}TRadioFlags;



//------------------------------------------------------------------------------
//  RF Frame Size Definitions
//------------------------------------------------------------------------------


//! maximum radio frame size - FIFO buffer Size, shared for RX and TX
#define RADIO_FRAME_PHY_PAYLOAD_SIZE    (255)

//! radio frame header size with network (group) address byte
//! 1 byte mac control field
//! 1 byte tx group address
//! 2 bytes destination address
//! 1 byte rx group address
//! 2 bytes source address
#define RADIO_FRAME_MAC_HEADER_SIZE     (7)

//! Size of CRC
#define RADIO_FRAME_MAC_CRC_SIZE        (2)

//! radio frame payload size
#define RADIO_FRAME_MAC_PAYLOAD_SIZE    (RADIO_FRAME_PHY_PAYLOAD_SIZE - RADIO_FRAME_MAC_HEADER_SIZE)

//! radio frame minimum size
#define RADIO_FRAME_MIN_SIZE            RADIO_FRAME_MAC_HEADER_SIZE

//! radio frame payload size available on application layer
#define RADIO_FRAME_APP_PAYLOAD_SIZE    RADIO_FRAME_MAC_PAYLOAD_SIZE


// Radio Message
typedef struct RadioMsg_T {
    //! Length Field
    uint8_t   Length;

    //! Control field
    uint8_t   Ctrl;

    //! Destination Group Address
    uint8_t   DstGroupAddr;

    //! Destination Address
    uint8_t   DstAddr[2];

    //! Source Group Address
    uint8_t   SrcGroupAddr;

    //! Source Address
    uint8_t   SrcAddr[2];

    //! Payload Field with Message dependent content
    uint8_t   Payload[RADIO_FRAME_MAC_PAYLOAD_SIZE];

    //! timestamp
    uint32_t  TimeStamp;

    //! RSSI value
    int16_t   Rssi;

    //! SNR value
    int8_t    Snr;

}TRadioMsg;

typedef void (*TRadioInterfaceCbMsgIndication)      (uint8_t* msg, uint8_t length, TRadioFlags trxFlags);
typedef void (*TRadioInterfaceCbLoRaWANHCIResponse) (uint8_t msgID, uint8_t* msg, uint8_t length);
typedef void (*TRadioInterfaceCbDevMgmtHCIResponse) (uint8_t msgID, uint8_t* msg, uint8_t length);


void iM880x_Init(void);

void iM880x_RegisterRadioCallbacks(TRadioInterfaceCbMsgIndication        cbMsgInd,
                              TRadioInterfaceCbLoRaWANHCIResponse   cbLoRaWANHCIRsp,
                              TRadioInterfaceCbDevMgmtHCIResponse   cbDevMgmtHCIRsp);

TWiMODLRResult iM880x_PingRequest();
TWiMDLRResultcodes iM880x_DirectDeviceActivation(uint32_t deviceAddress, uint8_t* nwkSessionKey, uint8_t* appSessionKey);
TWiMDLRResultcodes iM880x_SetJoinParameters(uint8_t* appEUI, uint8_t* deviceKey);
TWiMDLRResultcodes iM880x_JoinNetworkRequest(void);
TWiMDLRResultcodes iM880x_SendUDataTelegram(uint8_t* payload, uint16_t length);
TWiMDLRResultcodes iM880x_SendCDataTelegram(uint8_t* payload, uint16_t length);
TWiMODLRResult iM880x_SetRadioStackConfiguration(uint8_t SF,uint8_t flags);
TWiMODLRResult iM880x_DeactivateDevice();
TWiMDLRResultcodes iM880x_SetSystemOperationMode(uint8_t operationMode);
TWiMDLRResultcodes iM880x_SetDeviceEUI(uint8_t* deviceEUI);

#endif /* LORA_H_ */
