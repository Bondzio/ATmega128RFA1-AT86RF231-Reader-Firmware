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
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, data, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/
/*
  $Id: mac.c,v 1.1.1.1 2010/03/17 16:16:29 bleverett Exp $
*/
// Includes

#include <stdlib.h>
#include <stddef.h>
#include "hal.h"
#include "mac.h"
#include "radio.h"
#include "mac_scan.h"
#include "timer.h"
#include "mac_associate.h"
#include "rum_types.h"

#if __AVR32__
#include "avr32/hal_trx_avr32.h"
#endif

/**
   @addtogroup mac
   @{
*/


// Globals

#if DEBUG
/// Debugging temporary string, only used if DEBUG is non-zero
char debugStr[40];
#else
#define debugStr ((char *) 1)
#endif

/// MAC config struct, contains state info for the MAC
macConfig_t macConfig;

// Macros & Defines
/**
   @brief A global buffer to hold frames.  This area is used by both
   send an receive functions to hold radio frame data.  Sometimes the
   first byte is the frame length byte, and sometimes the first byte
   is the first byte of the frame.
*/
u8 mac_buffer_tx[sizeof(rx_frame_t)];  ///< Global MAC buffer (transmit)
u8 mac_buffer_rx[sizeof(rx_frame_t)];  ///< Global MAC buffer (receive)

// Implementation

/**
   Init the mac, which includes initializing the radio chip.

   @param Channel Sets the channel to use for the MAC.  Use 0xff for
   non-coordinator nodes.
*/
void macInit(u8 Channel)
{
#if __AVR32__
    // ETG DEBUG
    // Base address of the GPIO modules.
  //  volatile avr32_gpio_t * myGpio = &AVR32_GPIO;
#endif

    macConfig.panId = BROADCASTPANID;
    macConfig.shortAddress = BROADCASTADDR;
    macConfig.associated = false;

    macConfig.parentShortAddress = BROADCASTADDR;
    macConfig.lastRoute = BROADCASTADDR;
    macConfig.hopsToCoord = 0;
    macConfig.busy = 0;
    macConfig.sleeping = 1; // Assume sleeping state initially

    // Init radio
    radioInit(SERIAL ? true : false);

    // Set the channel
    macConfig.currentChannel = Channel;
    if (Channel != 0xff)
        radioSetOperatingChannel(Channel);

    radioUseAutoTxCrc(true);
    radioSetTrxState(TRX_OFF);

    // Set RF212 to 250KB mode.
    radioSetup900();

    // Set ack times shorter
   /// hal_subregister_write(SR_AACK_ACK_TIME, 1); //longer ack response time seems to work better, pb

    radioSetTrxState(RX_AACK_ON);

#if __AVR32__
    // DEBUG
//    myGpio->port[0].ovr &= ~(1<<0);
#endif

    // Setup the address of this device by reading a stored address from eeprom.
    halGetMacAddr((u8*)&(macConfig.longAddr));

    // Set up radio's coordinator flag
    radioSetDeviceRole(NODETYPE == COORD);

    // Setup radio's short addess
    radioSetShortAddress(BROADCASTADDR);

    // Setup radio's PANID
    radioSetPanId(BROADCASTPANID);

    // Set up the radio for auto mode operation.
    hal_subregister_write( SR_MAX_FRAME_RETRIES, 2 );

    // Load the long address into the radio. Needed for auto modes.
// ETG
    u64 longAddr = htons_64(macConfig.longAddr);
// ETG    radioSetExtendedAddress((uint8_t *)&macConfig.longAddr);
    radioSetExtendedAddress((uint8_t *)&longAddr);

    // Initialize the array of nodes (coordinator only)
    macInitNodes();
}

/**
   Set the radio's operating channel, and saves that channel to the
   MAC's global data structure.

   @param channel The channel number to use for radio communication.
*/
void macSetOperatingChannel(u8 channel)
{
    // Set the channel
    macConfig.currentChannel = channel;
    radioSetOperatingChannel(channel);
}

/** @} */
