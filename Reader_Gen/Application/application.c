/* Copyright (c) 2008  ATMEL Corporation
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/
/*
  $Id: application.c,v 1.113.2.1 2009/02/06 21:27:26 bleverett Exp $ 

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "radio.h"
#include "timer.h"
#include "mac.h"
#include "mac_event.h"
#include "mac_start.h"
#include "mac_data.h"
#include "mac_scan.h"
#include "mac_associate.h"
#include "system.h"
#include "hal.h"
#include "sixlowpan.h"
#include "sleep.h"
#include "application.h"
#include "system.h"
#include <avr/boot.h>
#include <avr/wdt.h>

#include "machine.h"
#include "eeprom_map.h"
#include "reader.h"
#include "display.h"
#include "BowComm.h"

// mifare includes
#include "RegCtrl.h"
#include "reg52.h"
#include "Mifare.h"
#include "Rc522Reg.h"
#include "OpCtrl.h"
#include "ErrCode.h"
#include "spi.h"

u8 DataBuffer[128];
u8 swipe = 0;
u8 TOFlag =0;
u8 KPTOFlag = 0;
u8 doItOnce = 1;
u16 LOOP_IN  =0;
u16 LOOP_OUT =0;
bool ADMIN_INIT = false;
bool ADMIN_CREDIT = false;
bool SEND_BALANCE = false;
bool INSUFFCIENT_FUNDS;

//extern u8 OP;
extern u8 ucGetMachineStatusFlag;

 u8 ucReturnBillTimeout=0;
//
bool SETUP_TIMEOUT = true;

#if (DEBUG_BOW)
  void DetermineState();
#endif // (DEBUG_BOW)

//Only the RUM stack uses these, otherwise undef them
#if !((IPV6_STACK == STACK_RUM) && IPV6LOWPAN)
#define sixlowpan_hc01_gen_rs(a)
#define sixlowpan_init()
#define sixlowpan_application_init()
#endif

//void sixlowpan_application_init(void);

extern bool Ping1Rx;
extern bool Ping2Rx;
extern bool Ping3Rx;
extern bool HaltValidation;
#if __AVR__
#include "serial.h"
#define euip_init_802154(a, b)
//#include <avr/eeprom.h>
// You can program the EEPROM with macaddress this way, using an ELF file
//u8 EEMEM macaddress[8] = {0xde, 0xed, 0xbe, 0xef, 0x11, 0x22, 0x33, 0x44};
//u8 EEMEM macaddress[8] = {0x01, 0x23, 0x45, 0x56, 0x78, 0x9a, 0xbc, 0xde};
//u8 EEMEM macaddress[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

static u8 pingTimer;

#endif // __avr__


u16 uiFunctionEntered = 1;
u8 ucLed1State = 0;
bool VALIDATION_TIMEOUT = false;

/**
   @addtogroup app
   @{

   This is a sample application, intended to show how the Route Under
   MAC can be used to create a simple application.

   @section app_network_start Joining the network

   On startup, this application uses a number of callback functions to
   bring up the network.  A coordinator node executes this process:

   - Call macFindClearChannel(), which starts a channel scan to find a
     free channel to use for the new PAN.

   - When the scan is complete, the appClearChanFound() callback is
     called. This then initializes the MAC/radio using the channel
     found.

   When a router or end node starts, it connects to the network using
   these steps:

   - appStartScan() is called, which scans all available channels,
     issuing a beacon request frame on each channel.

   - When the scan is complete, appScanConfirm() is called with an
     argument that specifies whether the scan successfully found a
     network to join.

   - If the scan process found a network, then appAssociate() is
     called, which causes the node to send an association request
     packet to the node found in the channel scan.

   - When an association response packet is received, the
     appAssociateConfirm() function is called by the MAC.  At this
     point, the node is part of the network and can send data packets
     to other nodes.

   @section app_data Sending and Receiving Data

   To send data to another node, the application can call the
   macDataRequest() function.  The application must know the address
   of the destination node, unless the destination is the coordinator,
   which always uses the short address 0x0000 (See @ref
   DEFAULT_COORD_ADDR).

   When a data packet is received, it is routed on to its
   destination. If the final destination is this node's short
   address, then the MAC will call the appDataIndication()
   function, which can then process the incoming data.
*/

// Set up function pointer to bootloader section.
void (*bootptr)( void ) = 0x1E000; 

#define TRACKDEMO 0

// Application variables
static u8 failCount;  ///< Number of transmit-to-parent failures before re-scanning.
volatile u8 gotPanCoordPing;
// Function declarations
void appStartScan(void);
void ledcallback(void);
void sixlowpan_button(void);
void ValidationTimeout(void);
char * GetParam(char *ptrBuffer, char *tmpstr, char *EndChar);
char  ParamStr[100];										   

#if __arm__
/**
    @brief This will toggle the led of choice on. Used by LED_ON().
*/
void ledOn(u8 val)
{
    if(1 == val) LED1ON();
    else if(2 == val) LED2ON();
    else if(3 == val) LED3ON();
    else if(4 == val) LED4ON();
}

/**
    @brief This will toggle the led of choice off. Used by LED_OFF().
*/
void ledOff(u8 val)
{
    if(1 == val) LED1OFF();
    else if(2 == val) LED2OFF();
    else if(3 == val) LED3OFF();
    else if(4 == val) LED4OFF();
}
#endif // __arm__



// These LED functions are used to provide address callbacks for macSetAlarm.
// Cannot use defines directly in callbacks.
void ledoff1(void)
{
    LED_OFF(1);
}

void ledoff2(void)
{
    LED_OFF(2);
}

void ledoff3(void)
{
    LED_OFF(3);
}

// This function can be used for diagnostic reasons. Not used in RUM application.
void blink1(void)
{
    LED_ON(1);
    macSetAlarm(25, ledoff1);

    //    macSetAlarm(500, blink1);

    //    macDataRequest(0, 7, (u8*)"Howdy\r\n");
}

/**
   Application callback function, called when the MAC receives a ping request
   packet addressed to this node.

   @param addr The short address of the node that sent the ping request
*/
void appPingReq(u16 addr)
{
	#if DEBUG
    // We got a ping, send a response
    // Blip the LED
    LED_ON(2);

    macSetAlarm( LED_PING_DELAY  ,ledoff2);
	//#endif
	
    debugMsgStrConst("\r\nPing request from node ");
    debugMsgInt(addr);
	#endif
	
    #if (PLATFORM == RAVEN && SERIAL)
        // Tell 3290p
        serial_send_frame(REPORT_PING_BEEP, 2, (u8*)&addr);
	#endif
    macPing(PING_RSP_FRAME, addr);
}
/**
   Application callback function, called when the MAC receives a ping
   response packet addressed to this node.

   @param addr The short address of the node that send the ping response.
 */

/**
   Application callback function, called when the MAC receives a ping
   response packet addressed to this node.

   @param addr The short address of the node that send the ping response
 */
void appPingRsp(u16 addr)
{
    debugMsgStrConst("\r\nPing response from ");
    debugMsgHex(addr);
    debugMsgStrConst(" LQI=");
    debugMsgInt(radioGetSavedLqiValue());
    debugMsgStrConst(" RSSI=");
    debugMsgInt(radioGetSavedRssiValue());
    debugMsgCrLf();

    #if (PLATFORM == RAVEN && SERIAL)
        // Tell 3290p
        serial_send_frame(REPORT_PING_BEEP, 2, (u8*)&addr);
	#endif
	
	#if DEBUG
    // turn on the LED
    LED_ON(3);
	   // and make it turn off after a litte bit
    macSetAlarm(LED_PING_DELAY,ledoff3);
	#endif
	
	if (radioGetSavedRssiValue())
	{
		macConfig.associated =  true;
	}
 
#if(__arm__)
    gotPanCoordPing = 1;
#endif
}


/**
   Application callback function.  Called when this node receives a
    ping response via IPv6
*/
void appSixlowpanPingResponse(void)
{
    #if (PLATFORM == RAVEN)
        // Send the 3290 a frame to show that we got a ping response.
        serial_send_frame(REPORT_PING_BEEP, 0, NULL);
	#endif
}

/**
   Application callback function.  Called when this node receives a
   UDP data packet.
*/
void appSixlowpanUdpResponse(char *str)
{
    #if (PLATFORM == RAVEN)
        // Send the 3290 a frame to show that we got a ping response.
        serial_send_frame(REPORT_TEXT_MSG, strlen(str), (u8*)str);
	#endif
}

/**
   Application callback function, called when the MAC receives an ACK
   packet from a node that we sent a packet to.
*/
void appPacketSendSucceed(void)
{
    // Reset the failure count on a good packet to parent
    // (could also decrement failCount)

    if (NODETYPE != COORD)
    {
        // figure out which way we were sending when the failure occurred
        if (macConfig.lastDestAddr == macConfig.parentShortAddress)
            failCount = 0;

        // Tell sensor app
        if (APP == SENSOR)
            sensorPacketSendSucceed();

        // Tell IPv6 LOWPAN
       // if (IPV6LOWPAN)
        //    sixlowpanSleep_packetSucceed();
    }
}

/**
   Application callback function, called when the MAC fails to send a
   packet due to a channel access failure (air is too busy).
*/
void appPacketSendAccessFail(void)
{
    // Tell sensor app
    #if (APP == SENSOR)
        sensorPacketSendFailed();
	#endif
}

/**
   Application callback function, called when the MAC fails to receive
   an ACK packet from a node that we sent a packet to.
*/
void appPacketSendFailed(void)
{

    //Tell 6LoWPAN
    #if IPV6LOWPAN
        sixlowpanSleep_packetFailed();
    #endif

    u8 parentFailed;
    if (NODETYPE != COORD)
    {
        // don't mess with assessing failures during scanning
        if (macIsScanning())
            return;

        debugMsgStrConst("\r\nFail=");
        debugMsgInt(failCount+1);

        parentFailed = (macConfig.lastDestAddr == macConfig.parentShortAddress);
        /* Special code: this prevents a situation where the
           coordinator goes away, and the nodes take a long time to
           realize that the network is gone.  Nodes Keep trying to
           re-associate with each other, but until a router loses a
           number of packets to its parent, it still thinks it's
           associated.  This code forces the issue by either verifying
           that the router is still connected, or forcing a failure.    */
        // Send an empty data packet
        #if(NODETYPE == ROUTER)
		if ( parentFailed )
            macDataRequest(macConfig.parentShortAddress, 0, NULL);
		#endif
        // Don't have a cow until we have missed a few packets in a row
        if (++failCount < 3)
        {
            // Tell sensor app
            if (APP == SENSOR)
                sensorPacketSendFailed();
            return;
        }

        // A sent packet failed to reach its destination too many times
        // figure out which way we were sending when the failure occurred
        if (parentFailed)
        {
            // re-associate if we were sending upstream
            macConfig.associated = false;

            // It is possible to make the coord/end units re-connect
            // to the coordinator more quickly by not scanning all
            // available channels.  To do this, uncomment the following
            // line.
             //macSetScanChannel(macConfig.currentChannel);

            if (APP == SENSOR)
                sensorLostNetwork();
            else
            macSetAlarm(((radioRandom(8)+50) *30)+1000, appStartScan);
        }
        if (NODETYPE == ROUTER &&
            macIsChild(macConfig.lastDestAddr))
        {
            // Drop child from table if the failure was downstream
            macRemoveChild(macConfig.lastDestAddr);
            debugMsgStrConst("\r\nDropped child bcs packet send failed.");
        }
    }

}

#if DEBUG || DOXYGEN
/**
    @brief This is an array of radio register names for serial output
           used when rf2xx_reg_dump() is called.  See the radio
           datasheet for details.
*/
u8 rf2xx_reg_names[][16] =
    {"TRX_STATUS", "TRX_STATE", "TRX_CTRL_0", "TRX_CTRL_1", "PHY_TX_PWR",
     "PHY_RSSI", "PHY_ED_LEVEL", "PHY_CC_CCA", "CCA_THRES", "TRX_CTRL_2", "IRQ_MASK",
     "IRQ_STATUS", "VREG_CTRL", "BATMON", "XOSC_CTRL", "RX_SYN", "RF_CTRL_0", "XAH_CTRL_1",
     "FTN_CTRL", "RF_CTRL_1", "PLL_CF", "PLL_DCU", "PART_NUM", "VERSION_NUM", "MAN_ID_0",
     "MAN_ID_1", "SHORT_ADDR_0", "SHORT_ADDR_1", "PAN_ID_0", "PAN_ID_1",
     "IEEE_ADDR_0", "IEEE_ADDR_1", "IEEE_ADDR_2", "IEEE_ADDR_3", "IEEE_ADDR_4",
     "IEEE_ADDR_5", "IEEE_ADDR_6", "IEEE_ADDR_7", "XAH_CTRL_0", "CSMA_SEED_0",
     "CSMA_SEED_1", "CSMA_BE"};

/**
    @brief This is an array of radio register values to be used when
           rf2xx_reg_dump() is called.  See the radio datasheet for
           details.
*/
reg_addr_t rf2xx_reg_enum[] =
    {RG_TRX_STATUS, RG_TRX_STATE, RG_TRX_CTRL_0, RG_TRX_CTRL_1, RG_PHY_TX_PWR,
     RG_PHY_RSSI, RG_PHY_ED_LEVEL, RG_PHY_CC_CCA, RG_CCA_THRES, RG_TRX_CTRL_2, RG_IRQ_MASK,
     RG_IRQ_STATUS, RG_VREG_CTRL, RG_BATMON, RG_XOSC_CTRL, RG_RX_SYN, RG_RF_CTRL_0, RG_XAH_CTRL_1,
     RG_FTN_CTRL, RG_RF_CTRL_0, RG_PLL_CF, RG_PLL_DCU, RG_PART_NUM, RG_VERSION_NUM, RG_MAN_ID_0,
     RG_MAN_ID_1, RG_SHORT_ADDR_0, RG_SHORT_ADDR_1, RG_PAN_ID_0, RG_PAN_ID_1,
     RG_IEEE_ADDR_0, RG_IEEE_ADDR_1, RG_IEEE_ADDR_2, RG_IEEE_ADDR_3, RG_IEEE_ADDR_4,
     RG_IEEE_ADDR_5, RG_IEEE_ADDR_6, RG_IEEE_ADDR_7, RG_XAH_CTRL_0, RG_CSMA_SEED_0,
     RG_CSMA_SEED_1, RG_CSMA_BE};
#endif

/**
    @brief Dumps the RF2xx register contents to serial port.

    Note: The serial output will only be available if the @ref DEBUG
    macro is defined as non-zero.
*/
void rf2xx_reg_dump(void)
{
#if DEBUG
    {
        u8 i,j,k,val;
        s8 str[40];

        debugMsgStrConst("\r\n\r\nREG DUMP\r\n");

        k = sizeof(rf2xx_reg_enum)/sizeof(rf2xx_reg_enum[0]);

        for(i=0;i<k;i++)
        {
            val =  hal_register_read(rf2xx_reg_enum[i]);
            sprintf((char*)str,"%-15s - %02X ", (char *)rf2xx_reg_names[i], (uint16_t)val);
            debugMsgStr((char *)str);
            for (j=7;j<8;j--)
                // Print a bit
                debugMsgChr(((val >> j)&1 ? '1' : '0' ));
            debugMsgCrLf();
        }

        debugMsgStrConst("\r\n");
    }
#endif
}

/**
   Application function, sends a data frame to the coordinator, and
   schedules another call in two seconds, using the timer function.

   This can be used for testing the network.  To send a real data
   frame, use macDataRequest().
 */
void appSendDataFrame(void)
{
    // send data frames once per second
    if (NODETYPE != COORD)
        macDataRequest(0x00, 4, (u8*) ((NODETYPE == ENDDEVICE) ? "endd" : "rout"));

    // Send another data frame later
    macSetAlarm(1000, appSendDataFrame);
}

/**
   Callback function, called when the MAC receives a data packet for
   this node.
 */
void appDataIndication(void)
{
    // Write app code here -- This node has received a data frame.

    // Example code:
/* typedef struct { */
/*     char buf[SAMPLE_COUNT/2];  // two samples/byte */
/*     short step_index; */
/*     short predicted_value; */
/* } tEncodedBuf; */
    // Find out the type of packet.


    u8 i;
    static u8 ucDataLength;

    // Find out the type of packet
    ftData *frame = (ftData *)(mac_buffer_rx+1);
    u8 size = *mac_buffer_rx;

	ucSendDataSize = 0;

     // End_Device Radio ENTRY POINT
	 // if (!size)
	if(size ==0)
        return;
    
    if ((frame->type & 0x7f) == DATA_FRAME)
    {

        // Get the length first.
        ucDataLength = frame->payload[0];
		memset(DataBuffer,0,sizeof(DataBuffer));

        for (i = 0; i < ucDataLength; i++)
        {
            DataBuffer[i] = frame->payload[i+1];
        }
        #if (DEBUG)
        Led3_on();
        #endif

		OP = extractIncomingMessage((char*)DataBuffer);
	
	    #if (DEBUG)
        Led3_off();
        #endif
		ucNumberOfRetry = 0;
    }

}

/** 
   \brief Function to send result back to PC through serial.
   \param ucDataLength is the data size.
   \param pucDataBuffer is the pointer to the data to be sent.
 */
#if (NODETYPE == COORD)
void Serial_send_data (u8 ucDataLength, u8* pucDataBuffer)
{
    u16 i;
    serial_putchar(SOP);    // Send SOP
    serial_putchar(ucDataLength);   // Send Length
    for (i = 0; i < ucDataLength * 2; i++)
    {
        serial_putchar(pucDataBuffer[i]);
    }
    serial_putchar(EOP);    // Send EOP
}


/** 
   \brief Convert data from Hexa to String.
   \param ucDataLength is the data size.
   \param pucDataBuffer is the pointer to the data to be converted.
 */

void Hexa_to_string_convert ( u8 ucDataLength, u8* pucDataBuffer )
{
    u8 i, ucDataBuffer[ucDataLength];
    for (i = 0; i < ucDataLength; i++)
    {
        ucDataBuffer[i] = pucDataBuffer[i];
    }

    for (i = 0; i < ucDataLength; i++)
    {
        pucDataBuffer[2 * i] = Nible_to_ascii ( ucDataBuffer[i] >> 4 );
        pucDataBuffer[(2 * i) + 1] = Nible_to_ascii ( ucDataBuffer[i] );
    }
}
#endif // (NODETYPE == COORD)

/**
   Example function, starts the network by scanning channels for a coordinator.

   This node will either scan all available channels, or just one
   channel if @ref macSetScanChannel() is called.  @see macScan().
*/
void appStartScan(void)
{
    if(NODETYPE != COORD)
    {
	   #ifdef DEBUG_RADIO
        debugMsgStrConst("\r\nStart scan ");
       #endif

		//macSetScanChannel(0x0f);//BOW is operating only on this channel
        macInit(0xff);
		
        macScan();
    }

    if (IPV6LOWPAN)
        sixlowpan_application_init();
}

/**
   Callback function, called when the MAC has associated a child of
   this node to the network.

   @param shortAddress The short address of the new child node that
   has been associated.  The MAC stores this address, so the
   application should not have to.
*/
void appChildAssociated(u16 shortAddress)
{
    // Blip the LED when we associate a child
    LED_ON(2);
    macSetAlarm(LED_DELAY,ledoff2);
}

/**
   Callback function, called when the MAC has associated a new node to
   a network. This is only called for the coordinator.

   @param shortAddress The short address assigned to the new node.  The
   MAC stores this address, so the application should not have to.
*/
void appNodeAssociated(u16 shortAddress)
{
}


/**
   Callback function, called when the MAC receives an association
   confirm packet addressed to this node, or the process timed out.

   @param success True if an association packet was received, or false
   if the association process timed out.
*/
void appAssociateConfirm(u8 success)
{
    if(NODETYPE != COORD)
    {
        if (success == SUCCESS)
        {
            // send data frame after a bit
			#ifdef DEBUG_RADIO
             debugMsgStrConst("\r\nAssociated to ");
             debugMsgHex(macConfig.parentShortAddress);
             debugMsgStrConst(" as ");
             debugMsgHex(macConfig.shortAddress);
			#endif


            LED_ON(2);
            macSetAlarm(LED_DELAY,ledoff2);

            // For raven, notify other processor
            #if ((PLATFORM == RAVEN) && SERIAL)
                serial_send_frame(REPORT_ASSOCIATED, 2, (u8*)&macConfig.shortAddress);
			#endif
			
            /* 6lowpan association */
            if (IPV6LOWPAN)
                sixlowpan_hc01_gen_rs();
				
            // If we are auto-sending data, start that process.
            #if ((APP == SENSOR) && (NODETYPE != COORD))
            {
                sensorInit();
                sensorAutoSendStart();
            }
			#endif
			
			#if TRACKDEMO
            // Track Demo Application
            if (NODETYPE == ENDDEVICE)
            {
                // Re-associate every second
                macSetAlarm(1000, appStartScan);
            }
			#endif
            #if ((APP == TRACKER)  && (PLATFORM == DSK001))
            {
                // Sleep for a while
                nodeSleep(30);
                // Do the scan over
                appStartScan();
            }
			#endif
        }
        else
        {
            debugMsgStrConst("\r\nFailed to associate");

            if (VLP)
            {
                // Sleep for 10 seconds, try again
                nodeSleep(100);
                appStartScan();
            }
            else
                // Try again in one second
                //macSetAlarm(1000, appStartScan);
				macSetAlarm(((radioRandom(8)+50) *30)+1000, appStartScan);
        }
    }
}

/**
   Sample application function, associates this node to a network.
   This function is called after a successful @ref macScan.
*/
void appAssociate(void)
{
  #ifdef DEBUG_RADIO
    debugMsgStrConst("\r\nAssociating to ");
    debugMsgHex(panDescriptor.coordAddr);
    debugMsgStrConst(" on ch ");
    debugMsgInt(panDescriptor.logicalChannel);
    debugMsgStrConst(", hops = ");
    debugMsgInt(panDescriptor.hopsToCoord);
  #endif

    macAssociate(panDescriptor.coordAddr, panDescriptor.logicalChannel);
}

/**
   Callback function, called when the MAC has completed its channel scan.

   @param success True if @ref macScan found a network to connect to,
   or false if no networks were found.
*/
void appScanConfirm(u8 success)
{
    // Write app code here -- This (end or router) node has finished its scan,
    // and has received some beacons from prospective parents.

    // Example code:
    if(NODETYPE != COORD)
    {
        if (success)
        {
            // associate with coordinator
            macSetAlarm(30,appAssociate);

            #ifdef BEBUG_RADIO
             debugMsgStrConst("\r\nScan good, select chan ");
             debugMsgInt(panDescriptor.logicalChannel);
			#endif
        }
        else
        {
            // failure to find a network
            if (VLP)
            {
                // Try again after sleeping for 10 seconds
                if (NODETYPE == ENDDEVICE)
                    nodeSleep(100);
                else
                    // Router node
                    macSetAlarm(1000, appStartScan);
            }
            else
                //macSetLongAlarm(30,appStartScan);// time between scan attempts
				macSetAlarm((radioRandom(8)+5) *10, appStartScan);
				
                debugMsgStrConst("\r\nScan bad");
        }
    }
}

/**
   Callback function, called when @ref macFindClearChannel()
   completes.

   u8 channel The clear channel selected for use by this PAN.
*/
void appClearChanFound(u8 channel)
{
    if (NODETYPE == COORD)
    {
        macInit(channel);
        macStartCoord();
#if (__AVR__ || __AVR32__)
        debugMsgStrConst("\r\nStartup, I am coordinator on ch ");
#else
        fnDebugMsg("\r\nStartup, I am coordinator on ch ");
#endif // __AVR__
        debugMsgInt(channel);
        macConfig.associated = true;

#if ((IPV6_STACK == STACK_UIP) && IPV6LOWPAN)
       //Start uIP stack with 802.15.4 interface
        huip_init_802154(macConfig.panId, macConfig.shortAddress);
#endif // IPV6LOWPAN
    }
}

#if __AVR__
/**
   Verifies that the EEPROM contains valid data for the stored MAC
   address.  If the EEPROM is unprogrammed, then a random MAC address
   is written into EEPROM.

   Similarly, the sensor interval time is set to 2 seconds if the
   EEPROM is unprogrammed.
 */
void checkEeprom(void)
{

	//check for First Run
	if(PLATFORM == RCBRFA1)
	{
	    u64 NodeLongAddress;
        u32 low;
		u32 high;
        u8 regval;
		u8  i=0;	        
		
		u8 temp = READER_NEW;

        
		
        // Setup the address of this device by reading a stored address from eeprom.
        halGetMacAddr((u8*)&(NodeLongAddress));
      
        // Partition the long address
        low = NodeLongAddress & 0xffffffff;
        high = NodeLongAddress >> 32;
        
        // Vigin Board Configuration Found, 
        // Request Setup Info from Reader

		if((low == 0xFFFFFFFF) && (high == 0xFFFFFFFF)){
		    // Produce Random Number
			
			    u8 buf[8];
				u8 bits =8;

				memset(&buf,0,sizeof(buf));
			    // create random MAC address and store it
	       		radioInit(0); // Needed for random to work.
	        	radioSetTrxState(RX_ON);

				
	        	for (u8 x = 0;x < 8;x++){

					      
				  if(x < 4)
					buf[x] = 0xFF;			// Load lower 4-bytes w/ 0xFF   

				  else{
				  
			        i = radioGetTrxState();

					// Load upper 4-bytes w/ random number (4-bytes)
			        if(i == RX_ON || i == RX_AACK_ON)
					     // Must be in rx to get random numbers
			            // Random number generator on-board
			            // has two random bits each read
			            for (i=0;i<bits/2;i++)
			            {
			                regval = hal_subregister_read(SR_RND_VALUE);
			                buf[x]= (buf[x] << 2) | regval;
			            }

					}
				}


			  halPutMacAddr(buf);
			  //ReaderStateFlag.FirstRun |= VIRGIN_READER;
			  
			  halPutEeprom(READER_STATE_ADDR,READER_STATE_FLAG_SIZE,&temp);
			  eeprom_update_byte(MACHINE_LABEL_ADDR, 0); //make label empty string
			  eeprom_update_byte(MACHINE_DESCRIPTION_ADDR, 0); //make description empty string
		}

        else if((low == 0xFFFFFFFF) && (high != 0x00000000)){
            //ReaderStateFlag.FirstRun |= VIRGIN_READER;
			halPutEeprom(READER_STATE_ADDR,READER_STATE_FLAG_SIZE,&temp);
			asm("nop");
			}
/*         else{// Valid (non-fffffff) MAC addr

            //a valid reader ID has been detected. check for valid setup
            if( (ReaderStateFlag.ReaderSetup == READER_SETUP_DONE) ){
                #if (DEVICE_CONNECTED == MDC_MACHINE)
                SetMachineSetupData();  
                #endif 
				
				ReaderStateFlag.ValidateSetup = VALIDATE_READER_SETUP;
				             
            }
            
                    	            
        }	*/	
		
    }
}
#endif // __AVR__



/**
   Sample application function, which initializes the application.
   This function is meant to be called on startup only.
*/
void appInit(void)
{
	
	#if (DEVICE_CONNECTED == MDC_MACHINE)
     u8 machineCode;
	 u8 tmp=0;
	 u8 machineTimerId;
	#endif
  
    // Init the mac
    LED_INIT();
    Leds_on();
    Buzzer_init();
    Leds_off();

    // Blip the LED once on powerup
    Led1_on();

    OP = VALIDATE_READER; 	
    macSetAlarm(25, ledoff1);

	ucDeviceStateFlag = DEVICE_STATUS_NEEDED;
    

#if (__AVR__)
    // If the EEPROM is cleared, init it to something useful
    checkEeprom();
#endif // __AVR__
	
	initReader(); 

	
    #ifdef MACHINE_CONNECTED
    
        #if (DEVICE_CONNECTED == MDC_MACHINE)
        //wait up to 10 seconds for machine to become active
         machineTimerId = macSetAlarm(MACHINE_WAIT_TIMEOUT,WaitForMachine);     
     
         while(ucMachineWait == MACHINE_WAIT)
         {
            if(ucDeviceStateFlag == DEVICE_STATUS_NEEDED)
            {
				initMachine();
        	   // MachineStatus();				
                tmp = macSetAlarm(DEVICE_POLL_PERIOD,SetDeviceState);
            }
            else if(ucDeviceStateFlag == DEVICE_STATUS_ONLINE){
                macTimerEnd(machineTimerId);
                macTimerEnd(tmp);
                //SetDefaultMachineSetup(); //load default setup info. these values will be overwritten when reader gets setup from BOW
                break;
            }
            else if( mac_event_pending() ){
                macTask();  //timer has timed out handle timer task.
            }
      
         }	 
         macSetAlarm(DEVICE_POLL_PERIOD,SetDeviceState);
        //#elif (DEVICE_CONNECTED == MAYTAG_MACHINE)
           // sendMTStartMessage();
           // ucDeviceStateFlag = DEVICE_STATUS_NEEDED;        

        #endif // DEVICE_CONNECTED
    #else
	  // FOR DEBUGGING USE ONLY
	 MdcStatus.MachineType[0] = 0x21;
	 MdcStatus.MachineType[1] = 1; 
  
    #endif
    /*
    #if (DEBUG == 0)
    //start heartbeat
    HeartBeat();
    #endif
    */
    #if (DEVICE_CONNECTED == MDC_MACHINE)
	// read machine code stored in EEPROM
	/********can check reader state flag here and update******/
	halGetEeprom(MACHINE_TYPE_ADDR, 1, &machineCode);
	
	//if a valid machine is connected, then store the machine type
	if((machineCode == 0xFF) && (MdcStatus.MachineType[0] != INVALID_MACHINE)) 
	{
		// save the actual machine code 
		halPutEeprom(MACHINE_TYPE_ADDR, 1, &MdcStatus.MachineType[0]);
		halPutEeprom(MACHINE_TYPE_ADDR+1, 1, &MdcStatus.MachineType[1]);
	}
	else if(MdcStatus.MachineType[0] != machineCode)
	{
		// machine has been swapped - trigger setup request
		halPutEeprom(MACHINE_TYPE_ADDR, 1, &MdcStatus.MachineType[0]);
        halPutEeprom(MACHINE_TYPE_ADDR+1, 1, &MdcStatus.MachineType[1]);
		
        ReaderStateFlag.ReaderSetup = READER_SETUP_NEEDED;//(ReaderStateFlag | READER_SETUP_NEEDED); 
        halPutEeprom(READER_STATE_ADDR,1, (u8*)&ReaderStateFlag);
	}    
    
    #endif //DEVICE_CONNECTED

    // Init the mac and stuff
    if (NODETYPE == COORD)
    {
        macFindClearChannel();
    }
    else
    {
	   #ifdef DEBUG_RADIO
        debugMsgStrConst("\r\nStartup, I am router/end.");
	   #endif

        // End node or router, start scanning for a coordinator
        appStartScan();
        // when scan is finished, appScanConfirm will be called.

        //Call RUM init
        sixlowpan_init();

        #ifndef COORDNODE

            #if (DEVICE_CONNECTED == MDC_MACHINE)
			if (ucDeviceStateFlag == DEVICE_STATUS_ONLINE)
			{
				displayVersion();
				
				//Send machine setup at every power up if reader has setup and machine is connected
				if( ReaderStateFlag.ReaderSetup == READER_SETUP_DONE )
				{					
					setupMachine();
					ucCardStatus = CARD_OUT;					
					cardRemoved(REMOVE_CARD);
					//collectMdcAuditData();		
				}
			}					
            #endif //MDC_MACHINE

        #endif

    }
	//ping coord every few seconds
	//pingTimer = 0;
	//doPing();
	

asm("nop");
}

/**
   Sample application function. Sends a ping packet to the network
   coordinator.
*/
void doPing(void)
{
    hal_register_write(RG_CSMA_BE,0);
    //macDataRequest(DEFAULT_COORD_ADDR, 110, dataString);
    macPing(PING_REQ_FRAME, DEFAULT_COORD_ADDR);

    // Uncomment the next line to make the button unleash a torrent of pings
#if __AVR__ || __AVR32__
    //    pingTimer = macSetAlarmMs(5+(23*(BAND == BAND900)), doPing);
	if (pingTimer)
	{
		macTimerEnd(pingTimer);
	}
    pingTimer = macSetAlarm(10000, doPing);
#endif
}


/** 
   \brief Convert data from String to Hexa.
   \param ucDataLength is the data size.
   \param pucDataBuffer is pointer to data to be converted.
 */

void String_to_hexa_convert ( u8 ucDataLength, u8 * pucDataBuffer )
{
    u8 i, ucDataBuffer[ucDataLength * 2];

    for (i = 0; i < ucDataLength * 2; i++)
    {
        ucDataBuffer[i] = pucDataBuffer[i];
    }
    for (i = 0; i < ucDataLength; i++)
    {
        ucDataBuffer[2 * i] = Ascii_to_nible ( ucDataBuffer[2 * i] );
        ucDataBuffer[(2 * i) + 1] = Ascii_to_nible ( ucDataBuffer[(2 * i) + 1] );
        pucDataBuffer[i] = ((ucDataBuffer[2 * i]) << 4) + ucDataBuffer[(2 * i) + 1];
    }
}


/**
   Sample application task loop function.  This function is meant to
   be called periodically.  It receives data from serial and sends it
   to end device.
*/
//void appTask(void)
void APP_TaskHandler(void)
{
	//vars declaration
	ucSendDataSize = 0;
	unsigned char tmp[16];

	u8 ucTimerID=0xff;
	char *uctempBuf;
	char *ptr;    

	// loop to convert 
	// decimal to hexadecimal	
	///u8 i =0;
	///u8 Nibble =0;
	u8 k, l;
	u8  Setup[30]; 
	u8 n = 0;

	
  #ifdef MACHINE_CONNECTED
	#if (DEVICE_CONNECTED == MDC_MACHINE)
	if(ucDeviceStateFlag == DEVICE_STATUS_NEEDED)	//do machine status communication every 1/2 second
	{
        DevicePoll();	
		
		
		if (ucDeviceStateFlag == DEVICE_STATUS_ONLINE)
		{	
			_delay_ms(5);
			//if (macConfig.associated == false)
			{
				//appPingReq(DEFAULT_COORD_ADDR);
				//appStartScan();
			}
			if (ReaderStateFlag.ReaderSetup == READER_SETUP_NEEDED)
			{
				//Display "No Setup" message on machine
				displayMsg(SETUP_MSG);				
			}
			else if (((macConfig.associated == false) && (ReaderStateFlag.EnableOfflineTransaction == false)) || (ReaderStateFlag.MaxNumTransReached == true))
			{
				//Display "Service message" message on machine
				displayMsg(SERVICE_MSG);				
			}
			else if (ReaderStateFlag.Busy == true)
			{
				#if (DEVICE_CONNECTED == MDC_MACHINE)
				//Display busy message
				if( OP != WAIT_FOR_SERVER ) // ****need to put op code in a queue****
				{
					OP = WAIT_FOR_SERVER;
				}					
				displayMsg(BUSY_MSG);
				#endif
			}
		}
		
	}
	#endif // MDC_MACHINE
	
  #endif

if(macConfig.associated == true/*/false*/ || ReaderStateFlag.EnableOfflineTransaction == true) //  Don't bother running the app until the reader has associated
{		
	switch(OP){
		case VALIDATE_READER:
		//TODO: fix machine type switch
		// 
				#if (DEVICE_CONNECTED == MDC_MACHINE) 
				if((ReaderStateFlag.EnableOfflineTransaction == true) && (ReaderStateFlag.ReaderSetup == READER_SETUP_DONE))
				{	
					OP = INIT_READER;
				}
				else{													
				#endif //MDC_MACHINE 
				
					#if (DEVICE_CONNECTED == MDC_MACHINE)
					if( ((ReaderStateFlag.ReaderSetup == READER_SETUP_NEEDED) || ( ReaderStateFlag.FirstRun == VIRGIN_READER)) && (MdcStatus.MachineType[0] != INVALID_MACHINE) ){
						
						SETUP_TIMEOUT = true;
						OP = SETUP_REQUEST; //enable when setup supported by server				
											
					#endif
	
					}
					else if( (ReaderStateFlag.ReaderSetup == READER_SETUP_DONE) && (ReaderStateFlag.ValidateSetup == VALIDATE_READER_SETUP) ){
                       	
						VALIDATION_TIMEOUT = false;
						sendValidationRequest();
						// Clear Rx Buffer
						memset(DataBuffer,0,sizeof(DataBuffer));
					   
						macSetLongAlarm(SETUP_VALIDATION_TIMEOUT, ValidationTimeout); 		//check Every 15s

						// Wait for validation package to arrive
						OP = VALIDATE_SETUP;
                        
					}
					else if( ReaderStateFlag.ReaderSetup == READER_SETUP_DONE){
						OP = INIT_READER;
					}
				#if( DEVICE_CONNECTED == MDC_MACHINE)
				}					
				#endif //MDC_MACHINE
			break;

		case INIT_READER:
			#if (DEVICE_CONNECTED == MDC_MACHINE)
			
			if( isMachineCycleRunning() )
			{
				OP = CYCLE_STARTED;
			}
			else
			{											
				OP = SCANNING;
			}
			
			#endif
			
			
			 break;

		case SCANNING: //scan for card swipe

                // Enable Heart beat routine
                    if (uiFunctionEntered < (20 * (F_CPU/4000000)))
                    {
                        uiFunctionEntered++;
                    }
                    else
                    {						
                        uiFunctionEntered = 0;					
                    }
	
					if(!Request(ISO14443_3_REQALL, tmp))
					{
						u8 cardType;
						u8 cardKey[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
						
						/*anti-collision and select the card*/
						if(!AnticollSelect(0, tmp, &cardType))
						{
							if(cardType == MIFARE_1K)
							{
								CurrentAccount.Type = MIFARE_1K;
								
								Authentication(MIFARE_AUTHENT_A,cardKey,tmp,0);
							}
							else 
							{
								CurrentAccount.Type = 0;
							}								
							OP = CARD_DETECTED;	
							ledoff1();
						}		
																	
						
					}				
				
				
                
				break;

		
		
		
		
		case CARD_DETECTED:		
			{			
				

				u8 cardNum[10] = {0};	
				//memset(DataBuffer,0,sizeof(DataBuffer));	// Clear Buffer
				
				
				//Read CardID from User Zone
				if(!readCardID(CurrentAccount.Type, &cardNum[0]))
				{
					padLeft((char*)&cardNum[0],9,"200");
					
					CurrentAccount.ID = strtoul((const char*)cardNum,NULL,10);	//save active card's id
					
					if(macConfig.associated == true)
					{					
						// Send Data Package via Radio						
						sprintf((char*)ucSendDataBuffer+1, "<sN>%s</sN>",cardNum);

						ucSendDataSize = strlen((char*)ucSendDataBuffer+1);
						ucSendDataBuffer[0] = ucSendDataSize;   // Length
						ucSendDataSize = ucSendDataSize + 1;    // data size = data byte + size byte
						macDataRequest(DEFAULT_COORD_ADDR, ucSendDataSize, ucSendDataBuffer);
									
						// Clear Rx Buffer
						memset(DataBuffer,0,sizeof(DataBuffer));
					
						OP = WAIT_FOR_SERVER;
						ReaderStateFlag.Busy = true;
						
					}
					#if (DEVICE_CONNECTED == MDC_MACHINE)


					else if((macConfig.associated == false) && (ReaderStateFlag.EnableOfflineTransaction == true) && (ReaderStateFlag.MaxNumTransReached == false))
					{//allow cycle to start when BOW is down
						CurrentAccount.Value = vendPrice * 100;				 
    					//CurrentAccount.CardState = CARD_IN;
    					if( cardInserted(REGULAR_VEND) )
    					{
							
							OP = WAIT_FOR_SELECTION;
							
							KPTOFlag = 0;
							
							if( waitForMachineStartKey() )
							{
    							macSetLongAlarm(MachineSetupData.KeyPressWaitTime,KeypressTimeOut);
							}
							else
								macSetAlarm(500, KeypressTimeOut); // if connected to a washer while cycle is running and a card is swiped, just display the balance and continue scanning.								
    					
						}							
					}
					else{
						OP = SCANNING;
					}
					#endif	
											                            
					#if (DEBUG)
						Beeps(1);
					#endif

					//Set Timeout timer
					TOFlag = 0;
					ucTimerID = macSetLongAlarm(SERVER_RESPONSE_TIMEOUT,CommTimeOut);
					
				}
				else
				{					
					#if (DEBUG)
						debugMsgStr("\r\nInvalid CardID\r\n");
					#endif
					OP = SCARD_REMOVED;
				}
				}		
			break;

		case SEND_OFFLINE_TRANSACTIONS:
		
			if (timeout.sendingOfflineTransactions)
			{
				timeout.sendingOfflineTransactions = false;
				OP = IDLE;
			}
			break;
			
		case SHOW_BALANCE_REQUEST:
		case WAIT_FOR_SERVER: //wait for response from server


		    if(DataBuffer[0] != '\0')
			{
			  // Print response
			
				ReaderStateFlag.Busy = false;
				#if (DEBUG_BOW)
				debugMsgStr("\r\n");
				debugMsgStr(DataBuffer);	
				debugMsgStr("\r\n");
				#endif

				OP = SCARD_REMOVED;

                #if (DEVICE_CONNECTED == MDC_MACHINE)
                    
                    //Extract the Card Id				
    				if( (uctempBuf = strstr((char *)DataBuffer,"sN: ")) )
                    {						
						CurrentAccount.ID = strtoul((const char*)(uctempBuf+4),NULL,10);	//save active card's id
						

								
    					//Display card balance on machine
    					double fBalance = 0.0;
    					uctempBuf = strstr((char *)DataBuffer,"Balance: ");
						
    					uctempBuf += 9;
    					ptr = strchr(uctempBuf,'.');
						k = ptr - uctempBuf;
						fBalance = atof(uctempBuf);
     
    					// Format the Balance to XX.XX
						if (k <2)
						 sprintf(uctempBuf,"0%.2f",fBalance);
						else if(k == 2)
						 sprintf(uctempBuf,"%.2f",fBalance);
						else
						 strcpy(uctempBuf,"99.99");
 
     					//Format the Display Balance
    					CurrentAccount.Value = (uctempBuf[0]-0x30) * 1000 + (uctempBuf[1]-0x30) * 100 + (uctempBuf[3]-0x30) * 10 + uctempBuf[4]-0x30;				 
    					//macTimerEnd(ucTimerID);


						#if(DEVICE_CONNECTED == MDC_MACHINE)
						
						// give display control back to front end controller.
						displayMsg(BLANK_MSG);
    					_delay_ms(100);
						
						if( cardInserted(REGULAR_VEND) )
    					{
    						#if(DEBUG_BOW)
    						debugMsgStr("\r\nSerialSendMachineData(CASH_CARD_INSERTED)\r\n");
    						#endif

    						//CurrentAccount.CardState = CARD_OUT;
    						OP = WAIT_FOR_SELECTION;
	
    					//Sound Buzzer
    						#if (DEBUG)
								Beeps(2);
							#endif

    						KPTOFlag = 0;
							
							if( waitForMachineStartKey() )
							{
    							macSetLongAlarm(MachineSetupData.KeyPressWaitTime,KeypressTimeOut);
							}
							else
								macSetAlarm(500, KeypressTimeOut); // if connected to a washer while cycle is running and a card is swiped, just display the balance and continue scanning.								
    					}
						#endif	//MDC_MACHINE
					}	//serial number found in server database
					else //serial number error from server database
					{
						CommTimeOut();//TOFlag = 1;	//force timeout condition
					}											
                #endif//MDC_MACHINE

			 }
			 else if(TOFlag == 1 || macConfig.associated == false) // server fails to respond in time
			 {
				#if(DEBUG_BOW)
			   	debugMsgStr("\r\nTOFlag == 1\r\n");
				#endif

				// Error Occured
				// Re-enter SCANNING mode

				#if(DEVICE_CONNECTED == MDC_MACHINE)
				if ( cardRemoved(REMOVE_CARD) )
				{
					//TOFlag = 1;
					ReaderStateFlag.Busy = false;
					//send error message to machine display
					displayMsg(CARD_ERR_MSG);				
					OP = IDLE;
				}
				
				#if (DEBUG_BOW)
				debugMsgStr("\r\nServer Communication Error\r\n");
				#endif                
				
                #endif // DEVICE_CONNECTED
			             
			 }

             //asm("nop");
			 break;

		case WAIT_FOR_SELECTION: //wait to see if cycle start has been requested by user

			#if(DEVICE_CONNECTED == MDC_MACHINE)
            if( (MdcStatus.CmdToReader == DEDUCT_VEND_COMMAND || MdcStatus.CmdToReader == DEDUCT_TOPOFF_COMMAND) )
			{
					OP = START_CYCLE; // set only if vend price deduction successful.

			}
			else if(KPTOFlag == 1)
			{			
				if ( cardRemoved(CASH_CARD) )
				{
					//CurrentAccount.CardState = CARD_OUT;
					OP = SCANNING;
				}
			}
			#endif
            			
			break;

		case START_CYCLE:	// send start cycle command to machine

            #if(DEVICE_CONNECTED == MDC_MACHINE)

			if( startMachineCycle() )//SerialSendMachineData(MACHINE_START_COMMAND) )
			{
				
				structTransaction vend;
				
				
				
				vend.CardId			= CurrentAccount.ID;
				vend.vendPrice		= vendPrice * 100;
				vend.LocationId		= ReaderSetup.locationId;
				vend.ManufactureId	= ReaderSetup.manufacturerId;
				vend.MachineId[0]	= deviceStatus.deviceType[0];
				vend.MachineId[1]	= deviceStatus.deviceType[1];
				
				
				
				if((macConfig.associated == false) && (ReaderStateFlag.EnableOfflineTransaction == true))
				{//must be offline transaction, store a record.
					vend.isOffline = true;
					storeOfflineTransaction(CurrentAccount.ID);					
				}
				else{
					vend.isOffline = false;	
					CurrentAccount.Value -= (vendPrice*100);
					sendCCTransaction(&vend);
				}				
				OP = CYCLE_STARTED;
				
				//let server know machine is in use.

			}
			
			cardRemoved(CASH_CARD);
			
				
            #endif

		    break;

		case CYCLE_STARTED: // WASH COMPLETED
			
            #if(DEVICE_CONNECTED == MDC_MACHINE)
		/*	if( (MdcStatus.MachineStatus[0] & MACHINE_END_MODE) || (MdcStatus.MachineStatus[0] == MACHINE_ERROR_MODE))
			{*/
                //Error mode must be handled
                //send status to server.
				//LED_ON(2); //indicate cycle is running
				OP = SCANNING;
		/*	}
			else{   //check for server communication request or cancellation request from service tech

            }*/
            
            #endif
                
			break;



		case TIME_OUT: // Server Communication Error Occurred


              break;
		case SETUP_REQUEST:
		
			if( SETUP_TIMEOUT == true)
			{				
				#if(DEVICE_CONNECTED == MDC_MACHINE)
            
    			#ifdef MACHINE_CONNECTED
    			DevicePoll();
				/*if( MdcStatus.MachineType == INVALID_MACHINE )
				{   //Error with machine code; error mode must be handled.
					OP = SCANNING;
					break;
				}*/
				#else
    			deviceStatus.deviceType[0] = 33;
				deviceStatus.deviceType[1] = 1;
    			#endif
				#endif //DEVICE_CONNECTED
				sendSetupRequest();
			
				SETUP_TIMEOUT = false;
				macSetLongAlarm(SETUP_REQUEST_TIMEOUT, SetupTimeout);
			}			
			 break;

		
		//case SETUP_PACKAGE:
          //{
//
          //}
			//break;
			
		case VALIDATE_SETUP: 

		    	if((ptr = GetParam((char *)DataBuffer,"<ValidationResponse>","<")) != NULL)
				{  
				

					  u8 addr[8];
                      static u8 Len =0;
					  n = strlen(ptr);
				      k = 0;
					  l = 0;
					  			
				    // Get length of Validation code
				   		halGetEeprom(VALIDATION_CODE_ADDR,1,&Len);
																    
				   // 	Get Validation Code from EEPROM
						halGetEeprom(((u8*)VALIDATION_CODE_ADDR+1),Len,addr);
 
						while (k < n)
						{
							Setup[l] = (Ascii_to_nible ( *(ptr+k) ) * 16) 
									 +  Ascii_to_nible ( *(ptr+k+1));
							k += 2;
							l++;
						}



				    if(memcmp(addr,Setup,Len) == 0)
					{
                        ReaderStateFlag.ValidateSetup = VALID_READER_SETUP; //Don't store this bit in eeprom it allows for validation check on every power cycle.
			            OP = VALIDATE_READER;
					}						
					else
					{
					 OP = SETUP_REQUEST; // validation failed, request new setup info
                     SETUP_TIMEOUT = true;
					}					 
				 }

                     // Reposnse TIMEOUT
				if(VALIDATION_TIMEOUT)
				{
					VALIDATION_TIMEOUT = false;
					sendValidationRequest();// Re-send Validation Request
					
					macSetLongAlarm(SETUP_VALIDATION_TIMEOUT, ValidationTimeout); 		//check Every 5s
				}	

				   
				    			
			break;

        case UPDATE_READER_FIRMWARE:
            //process firmware update request from BOW
            
            break;

  
        case IDLE:
			#if( DEVICE_CONNECTED == MDC_MACHINE )				
				OP = SCANNING;
			#endif
			
			break;
		case SCARD_REMOVED:

            #if(DEVICE_CONNECTED == MDC_MACHINE)
			ucCardStatus = CARD_OUT;
			cardRemoved(REMOVE_CARD);
			//SerialSendMachineData(CARD_REMOVED);
			#endif

			#if( DEVICE_CONNECTED == MDC_MACHINE )				
				OP = SCANNING;
			#endif
			
			break;
		case BOW_ERROR:
			OP = IDLE;
			break;
			
		case APP_INIT:
			appInit();
			break;
			
		default:
				break;
} //switch


}// macConfig.associated

}

void ValidationTimeout()
{ VALIDATION_TIMEOUT = true;}

char * GetParam(char *ptrBuffer, char *tmpstr, char *EndChar)
{

  char *pStr;
  char *pStr2;
  u8 tmpstrlen =0;


  tmpstrlen = strlen(tmpstr);

   if((pStr = strstr(ptrBuffer,tmpstr)) != NULL){ //1
       if((pStr2 = strstr(pStr + tmpstrlen,EndChar)) != NULL){ //2 "<"
             memcpy(ParamStr,pStr + tmpstrlen, pStr2-(pStr+tmpstrlen));
              ParamStr[pStr2-(pStr + tmpstrlen)]  ='\0';

              return (char *)ParamStr;
        }//1
    }//2
 
 return '\0';
}

/**
   Callback function, called when the MAC finds no clear channel.
   This function will send packet again until the maximum retry number is reached.
 */
void appResendPacket(void)
{
#if (NODETYPE == COORD)
    associatedNodes_t *node;
	u64 *ulLongAddress;
    u8 ucResult[2];

    // Command has not been sent because clear channel was not found, try to re-send.
	if (ucNumberOfRetry < MAX_RETRY_TO_SEND_DATA)
	{
		node = macGetNode(1);
		ulLongAddress = &(node->nodeLongAddress); // Get node 1 long address
    	macDataRequest(macGetNodeAddr(ulLongAddress), ucSerialReceiveDataBuffer[0] + 1, (u8*) ucSerialReceiveDataBuffer);
		ucNumberOfRetry++;
        debugMsgStr("Send command again\n\r");
		// Re-start timer to wait for result.
        Timer2_set ( 0x00, 0x07 );
	}
	else
	{
        // Can not send command.
		Timer2_stop ();
        ucResult[0] = CAN_NOT_SEND_CMD;
        Hexa_to_string_convert(1, ucResult);
        // Send error code
        // When sending data to serial, the total of data must not be multiple 
        // of 16 bytes otherwise the data will not directly displayed by serial 
        // terminal.
        Serial_send_data (1, ucResult);
		ucSendCmdState = START;
	}

#elif (NODETYPE == ENDDEVICE)
    // Command has not been sent because clear channel was not found, try to re-send.
	if (ucNumberOfRetry < MAX_RETRY_TO_SEND_DATA)
	{
    	macDataRequest(DEFAULT_COORD_ADDR, ucSendDataSize, ucSendDataBuffer);
#if (DEBUG)
        debugMsgStr("Send result again\n\r");
#endif
	}
    else
    {
#if (DEBUG)
        // Can not send result.
        debugMsgStr("Can't send result\n\r");
#endif
    }
#endif
}
/** @} */

#ifndef COORDNODE 

 void Beeps(u8 nBeeps)
  {
	   for(u8 i= 0; i < nBeeps;i++)
		{
			for(u16 i= 0; i < 100;i++)
		{
			 Buzzer_on();
			_delay_us(200);
			Buzzer_off();
			_delay_us(200);
		}
		_delay_ms(25);
	}
}

void CommTimeOut()
{
	TOFlag = true;
	ReaderStateFlag.Busy = false; 
}

void KeypressTimeOut()
{
	KPTOFlag = true;
}

void HeartBeat()
{
    static u8 ucHeartBeatTimer = 0;
    if( ucHeartBeatTimer != 0 )
        macTimerEnd(ucHeartBeatTimer);
    Led2_toggle();
   ucHeartBeatTimer = macSetAlarm(HEARTBEAT, HeartBeat);
}

void SetupTimeout()
{
	SETUP_TIMEOUT = true;
}

#if (DEBUG_BOW)

void DetermineState()
{

   switch (OP)
   {

	case SETUP_RESPONSE:
        debugMsgStr("\n\rSETUP_RESPONSE:\n\r");
		break;	

	case SETUP_REQUEST:
        debugMsgStr("\n\rSETUP_REQUEST:\n\r");
		break;

	case SETUP_PACKAGE:
        debugMsgStr("\n\rUPDATE_READERID:\n\r");
		break;

	case INIT_READER:
	    debugMsgStr("\n\rINIT_READER:\n\r");
		break;
		
	case SCANNING:
		debugMsgStr("\n\rSCANNING:\n\r");
		break;		

	case CARD_DETECTED:
	    debugMsgStr("\n\rCARD_DETECTED:\n\r");
		break;	

	case CYCLE_STARTED:
	    debugMsgStr("\n\rCYCLE_STARTED:\n\r");
		break;		

	case SCAN_ENABLE:
	    debugMsgStr("\n\rSCAN_ENABLE:\n\r");
		break;			

	case DISABLE_SCAN:
	    debugMsgStr("\n\rDISABLE_SCAN:\n\r");
		break;			

	case WAIT_FOR_RESPONSE:
	    debugMsgStr("\n\rWAIT_FOR_RESPONSE:\n\r");
		break;		

	case TIME_OUT:
	    debugMsgStr("\n\rTIME_OUT:\n\r");
		break;				

	case WAIT_FOR_SELECTION: 
	    debugMsgStr("\n\rWAIT_FOR_SELECTION:\n\r");
		break;	 

	case ENABLE_SCAN:
	    debugMsgStr("\n\rENABLE_SCAN:\n\r");
		break;					

	case SCARD_REMOVED:	
	    debugMsgStr("\n\rSCARD_REMOVED:\n\r");
		break;		

	case START_CYCLE:			
	    debugMsgStr("\n\rSTART_CYCLE:\n\r");
		break;

     default:
    break;
	 
	}

}
#endif // (DEBUG_BOW)

void ProcessServerResponse(void)
{

}


void SetDeviceState(void)
{
	ucDeviceStateFlag = DEVICE_STATUS_NEEDED;
}

void DevicePoll(void) 
{
	 #if (DEVICE_CONNECTED == MDC_MACHINE)
  //  #if (DEVICE_CONNECTED == MDC_MACHINE || DEVICE_CONNECTED == MAYTAG_MACHINE)

        MachineStatus();
		macSetAlarm(DEVICE_POLL_PERIOD,SetDeviceState);
		//ucDeviceStateFlag = DEVICE_STATUS_DONE;	
    
    #endif

}

void SetBillReturn(void)
{
    ucReturnBillTimeout = 2;
}

char * padLeft(char * string, u8 paddedLength, const char * pad) 
{
	size_t stringLength = strlen(string);
	size_t lenpad = strlen(pad);
	u8 i = paddedLength;
	
	if (stringLength >= paddedLength)
	{
		return NULL;//'\0';
	}

	char * padded = (char*)malloc(paddedLength + 1); // allocate memory for new string
	
	for(; paddedLength > stringLength; paddedLength--, padded += lenpad)
	{
		strncpy(padded, pad, lenpad);
	}
	
	strncpy(padded, string, stringLength); /* copy without '\0' */
	padded += stringLength; /* prepare for first append of pad */
	*padded = '\0';

	padded = (padded - i);
	strncpy(string,padded,i);
	
	return padded;
}
#endif // ifndef COORDNODE


