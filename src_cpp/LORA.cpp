/*
 * LORA.cpp
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

#include <LORA.h>
#include "SLIP.h"
#include "CRC16.h"

TWiMODLR_HCIMessage TxMessage;
TWiMODLR_HCIMessage RxMessage;


TRadioInterfaceCbMsgIndication          cbMsgIndication;
TRadioInterfaceCbLoRaWANHCIResponse     cbLoRaWANHCIResponse;
TRadioInterfaceCbDevMgmtHCIResponse     cbDevMgmtHCIResponse;


static uint8_t configBuffer[4];

int iM880x_SendHCIMessage(uint8_t sapID, uint8_t msgID, uint8_t* payload, uint16_t length) {
    // 1. check parameter
    //
    // 1.1 check length
    //
    if(length > WIMODLR_HCI_MSG_PAYLOAD_SIZE) {
        return WiMODLR_RESULT_PAYLOAD_LENGTH_ERROR;
    }

    // 2.  init TxMessage
    //
    // 2.1 init SAP ID
    //
    TxMessage.SapID = sapID;

    // 2.2 init Msg ID
    //
    TxMessage.MsgID = msgID;

    // 2.3 copy payload, if present
    //
    if(payload && length) {
        uint8_t*  dstPtr  = TxMessage.Payload;
        int     n       = (int)length;

        // copy bytes
        while(n--)
            *dstPtr++ = *payload++;
    }

    // 3. Calculate CRC16 over header and optional payload
    //
    uint16_t crc16 = CRC16_Calc(&TxMessage.SapID, length + WIMODLR_HCI_MSG_HEADER_SIZE, CRC16_INIT_VALUE);

    // 3.1 get 1's complement
    //
    crc16 = ~crc16;

    // 3.2 attach CRC16 and correct length, lobyte first
    //
    TxMessage.Payload[length++] = (uint8_t)(crc16 & 0x00FF);
    TxMessage.Payload[length++] = (uint8_t)(crc16 >> 8);

    // 4. forward message to SLIP layer
    //    - start transmission with SAP ID
    //    - correct length by header size

    if (ComSlip_SendMessage(&TxMessage.SapID, length + WIMODLR_HCI_MSG_HEADER_SIZE)) {
        // ok !
        return WiMODLR_RESULT_OK;
    }

    // error - SLIP layer couldn't sent message
    return -1;
}


TWiMDLRResultcodes iM880x_SendUDataTelegram(uint8_t* payload, uint16_t length) {
    TxMessage.Payload[0] = LORA_MAC_PORT;

    if(payload && length) {
        uint8_t*  dstPtr  = TxMessage.Payload + 1;
        int     n       = (int)length;

        // copy bytes
        while(n--)
            *dstPtr++ = *payload++;
    }

    return (TWiMDLRResultcodes) iM880x_SendHCIMessage(LORAWAN_ID, LORAWAN_MSG_SEND_UDATA_REQ, nullptr, length);
}


TWiMDLRResultcodes iM880x_SendCDataTelegram(uint8_t* payload, uint16_t length)
{
    TxMessage.Payload[0] = LORA_MAC_PORT;

    if(payload && length)
    {
        uint8_t*  dstPtr  = TxMessage.Payload + 1;
        int     n       = (int)length;

        // copy bytes
        while(n--)
            *dstPtr++ = *payload++;
    }

    return (TWiMDLRResultcodes) iM880x_SendHCIMessage(LORAWAN_ID, LORAWAN_MSG_SEND_CDATA_REQ, nullptr, length);
}

TWiMDLRResultcodes iM880x_DirectDeviceActivation(uint32_t deviceAddress, uint8_t* nwkSessionKey, uint8_t* appSessionKey)
{
	//HTON32
    TxMessage.Payload[0] = (deviceAddress >> 0) & 0xFF;
    TxMessage.Payload[1] = (deviceAddress >> 8) & 0xFF;
    TxMessage.Payload[2] = (deviceAddress >> 16) & 0xFF;
    TxMessage.Payload[3] = (deviceAddress >> 24) & 0xFF;

    if(nwkSessionKey) {
        uint8_t*  dstPtr  = TxMessage.Payload + DEVICE_ADDR_LEN;
        int     n       = KEY_LEN;

        // copy bytes
        while(n--)
            *dstPtr++ = *nwkSessionKey++;
    }

    if(appSessionKey) {
        uint8_t*  dstPtr  = TxMessage.Payload + DEVICE_ADDR_LEN + KEY_LEN;
        int     n       = KEY_LEN;

        // copy bytes
        while(n--)
            *dstPtr++ = *appSessionKey++;
    }
    return (TWiMDLRResultcodes) iM880x_SendHCIMessage(LORAWAN_ID, LORAWAN_MSG_ACTIVATE_DEVICE_REQ, nullptr, DEVICE_ADDR_LEN + KEY_LEN + KEY_LEN);
}

TWiMDLRResultcodes iM880x_SetJoinParameters(uint8_t* appEUI, uint8_t* deviceKey)
{

    if(appEUI)
    {
        uint8_t*  dstPtr  = TxMessage.Payload;
        int     n       = EUI_LEN;

        // copy bytes
        while(n--)
            *dstPtr++ = *appEUI++;
    }

    if(deviceKey)
    {
        uint8_t*  dstPtr  = TxMessage.Payload + EUI_LEN;
        int     n       = KEY_LEN;

        // copy bytes
        while(n--)
            *dstPtr++ = *deviceKey++;
    }

    return (TWiMDLRResultcodes) iM880x_SendHCIMessage(LORAWAN_ID, LORAWAN_MSG_SET_JOIN_PARAM_REQ, nullptr, EUI_LEN + KEY_LEN);
}

TWiMDLRResultcodes iM880x_JoinNetworkRequest(void)
{
    return (TWiMDLRResultcodes) iM880x_SendHCIMessage(LORAWAN_ID, LORAWAN_MSG_JOIN_NETWORK_REQ, nullptr, 0);
}

TWiMODLRResult iM880x_PingRequest()
{
    return (TWiMDLRResultcodes) iM880x_SendHCIMessage(DEVMGMT_ID, DEVMGMT_MSG_PING_REQ, nullptr, 0);
}

TWiMODLRResult iM880x_SetRadioStackConfiguration(uint8_t SF,uint8_t flags)
{
    uint8_t offset=0;
    configBuffer[offset++]   =  3;      // SF 9
    configBuffer[offset++]   = 18;      // Tx Power level [dBm]
    configBuffer[offset++]   =  0;      // 00000000
                                        // ||||||||
                                        // |||||||- ADR disabled
                                        // ||||||-- Duty Cycle Control disabled
                                        // |||||--- Class A selected
                                        // -------- Rx MAC Command Forwarding disabled
    configBuffer[offset++]   =  0;      // Automatic Power Saving off
    configBuffer[offset++]   =  1;      // Number of Retransmissions
    configBuffer[offset++]   =  1;      // Radio Band Selection (1 == EU)

    // Set Configuration
    return iM880x_SendHCIMessage(LORAWAN_ID, LORAWAN_MSG_SET_RSTACK_CONFIG_REQ, (unsigned char*)&configBuffer, 6);
}

TWiMODLRResult iM880x_DeactivateDevice()
{
	return iM880x_SendHCIMessage(LORAWAN_ID, LORAWAN_MSG_DEACTIVATE_DEVICE_REQ, nullptr, 0);
}

TWiMDLRResultcodes iM880x_SetSystemOperationMode(uint8_t operationMode)
{
    uint8_t*  dstPtr  = TxMessage.Payload;
    *dstPtr = operationMode;
    return (TWiMDLRResultcodes) iM880x_SendHCIMessage(DEVMGMT_ID, DEVMGMT_MSG_SET_OPMODE_REQ, nullptr, 1);
}

TWiMDLRResultcodes iM880x_SetDeviceEUI(uint8_t* deviceEUI)
{
    if(deviceEUI)
    {
        uint8_t*  dstPtr  = TxMessage.Payload;
        int     n       = EUI_LEN;

        // copy bytes
        while(n--)
            *dstPtr++ = *deviceEUI++;
    }

    return (TWiMDLRResultcodes) iM880x_SendHCIMessage(LORAWAN_ID, LORAWAN_MSG_SET_DEVICE_EUI_REQ, nullptr, EUI_LEN);
}

uint8_t* iM880x_CbProcessRxMessage(uint8_t* rxBuffer, uint16_t length)
{
    // 1. check CRC
    if (CRC16_Check(rxBuffer, length, CRC16_INIT_VALUE))
    {
        // 2. check min length, 2 bytes for SapID + MsgID + 2 bytes CRC16
        if(length >= (WIMODLR_HCI_MSG_HEADER_SIZE + WIMODLR_HCI_MSG_FCS_SIZE))
        {
            // add length
            RxMessage.Length = length - (WIMODLR_HCI_MSG_HEADER_SIZE + WIMODLR_HCI_MSG_FCS_SIZE);

            // dispatch completed RxMessage
            // 3. forward message according to SapID / MsgID
            switch(RxMessage.SapID)
            {
                case    DEVMGMT_ID:

                        // forward Msg IDs to application, e.g. ping response
                        (*cbLoRaWANHCIResponse)(RxMessage.MsgID, RxMessage.Payload, length);

                        break;

                case    LORAWAN_ID:
                        // handle TX indications
                        if((RxMessage.MsgID == LORAWAN_MSG_SEND_UDATA_TX_IND)   ||
                           (RxMessage.MsgID == LORAWAN_MSG_SEND_CDATA_TX_IND)   ||
                           (RxMessage.MsgID == LORAWAN_MSG_JOIN_NETWORK_TX_IND))
                        {
                            (*cbMsgIndication)(nullptr, 0, trx_TXDone);
                        }
                        // handle RX messages
                        else if((RxMessage.MsgID == LORAWAN_MSG_RECV_UDATA_IND) ||
                                (RxMessage.MsgID == LORAWAN_MSG_RECV_CDATA_IND))
                        {
                            (*cbMsgIndication)(RxMessage.Payload, length, trx_RXDone);
                        }

                        // handle ACKs
                        else if(RxMessage.MsgID == LORAWAN_MSG_RECV_ACK_IND)
                        {
                            (*cbMsgIndication)(nullptr, 0, trx_ACKDone);
                        }

                        // handle other responses
                        else
                        {
                            (*cbLoRaWANHCIResponse)(RxMessage.MsgID, RxMessage.Payload, length);
                        }

                        break;

                default:
                        // handle unsupported SapIDs here
                        break;
            }
        }
    }
    else
    {
        // handle CRC error
    }

    // return same buffer again, keep receiver enabled
    return &RxMessage.SapID;

}


//------------------------------------------------------------------------------
//
//  iM880x_Init
//
//  @brief: Initialize radio interface
//
//------------------------------------------------------------------------------

void iM880x_Init(void)
{
    // Init Slip Layer
    ComSlip_Init();
    ComSlip_RegisterClient(iM880x_CbProcessRxMessage);

    // pass first RxBuffer and enable receiver/decoder
    ComSlip_SetRxBuffer(&RxMessage.SapID, (uint16_t)WIMODLR_HCI_RX_MESSAGE_SIZE);
}

void iM880x_RegisterRadioCallbacks(TRadioInterfaceCbMsgIndication        cbMsgInd,
                              TRadioInterfaceCbLoRaWANHCIResponse   cbLoRaWANHCIRsp,
                              TRadioInterfaceCbDevMgmtHCIResponse   cbDevMgmtHCIRsp)
{
    cbMsgIndication      = cbMsgInd;
    cbLoRaWANHCIResponse = cbLoRaWANHCIRsp;
    cbDevMgmtHCIResponse = cbDevMgmtHCIRsp;
}






