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
/**
   @mainpage RUM (Route-Under-MAC) network

   @section Quickstart

   There is a document available that guides you through getting the
   software working on a variety of hardware platforms.

   @section Introduction

   The RUM network is a simple, small, multi-hop, self-forming
   network.  It has the following characteristics:

   - Small object size.  Current implementation (MAC with tiny app) is
     about 5.5KB for an end node.

   - Coordinator is used.  The coordinator is the only node that has
     any "smarts", and the routers and end-nodes are quite simple.
     This allows for cheap hardware for both routers and end-nodes.

   - Self-forming network.  The nodes wake up and can find a network
     and associate to it.

   - Multi-hop routing is supported.  There is no practical limit to
     the number of hops.

   - Open protocol.

   @section comps Components

   There are a few modules to this project

   - @ref app provided.

   - @ref mac.  A subset of the MAC defined by IEEE 802.15.4.  This
     includes multi-hop and self-forming functionality.

   - @ref radio between the MAC layer to the Atmel AT86RF2xx radio chip.

   - @ref timer_module to support MAC events and any user-created events.

   - @ref serial code to support debugging or a serial connection to
     another device.  This code can be left out of the project to
     reduce object code size.

   - @ref sensors to demonstrate a simple data-collection network,
     including wireless calibration of sensors.


   @section esplanade How the components work together

   The example application runs on "bare metal", that is, without any
   kind of operating system.  In main(), there is a "forever" loop
   that continously calls the two program "threads", appTask() and
   macTask().  These tasks process the events that occur in the
   background, such as the timer interrupt and the radio interrupt.

   The interrupt routines that handle radio and timer events post an
   event object into the event queue (see @ref mac_put_event()).  The
   macTask() function checks this queue and processes posted events,
   most of which were initiated by an interrupt service routine (ISR).

   The @ref serial runs by itself in a stand-alone way.  The only
   interaction between the serial module and the rest of the program
   is through functions like serial_getchar() and serial_putchar().  A
   serial queue is used for both sending and receiving characters over
   the serial port.

   The @ref timer_module implements an easy-to-use interface to allow
   delayed processing of events.  This module is available for use by
   the 

   The @ref radio provides a device driver for the radio chip.
   This layer resides below the MAC layer, and the two layers talk
   directly to each other.

   @section compiling Compiling

   This package can be compiled on several host platforms, with many
   options for the target environment.  The code is written for the
   GCC compiler (ver 4.3.0), which has been compiled as a cross
   compiler for AVR.  See the avr-libc project
   http://www.nongnu.org/avr-libc/ for details.

   @subsection hostwin Windows Host Platform

   Compile and program the code into the target using Atmel's AVR
   Studio (available free at http://www.atmel.com).  You will also
   have to install WinAVR, which is the AVR GCC toolchain compiled for
   a Win32 host.

   There are AVR Studio project files supplied for the various builds.
   Load the project file and press the compile button.  If you have a
   debugger connected to the target, you may load the code into the
   target and begin debugging.

   @subsection hostlin Linux Host Platform

   This package includes a Makefile.linux file.  To compile RUM on a
   linux platform, issue a command like

   @code
   make -f Makefile.linux PLATFORM=RCB230 DEBUG=0 TYPE=ROUTER
   @endcode

   This makefile accepts a number of parameters as shown above.  The
   list of available parameters includes:

   - TYPE can be END, ROUTER, or COORD.  This directs which kind of
     node is being compiled.  This parameter is required.

   - PLATFORM can be RCB230, RCB231, or RAVEN.  This sets the MCU flag
     and other parameters to compile the code for a specific platform.
     If this parameter is left out, the PLATFORM will be set to the
     default.  Note that a platform defines how the CPU and radio chip
     are connected together. It does not specify which CPU or radio
     chip is being used.

   - DEBUG can be 1 or 0.  This enables/disables diagnostic messages
     from the serial port.  Disabling DEBUG will greatly reduce the
     object code size (flash size).

   - DEMO can be 1 or 0.  In DEMO mode, a node picks its parent node
     as the one with the highest RSSI.  Otherwise, a parent is picked
     by (in sequence) best LQI, lowest number of hops to the
     coordinator, and then RSSI.

   - APP can be 0 or 1 (@ref SENSOR).  If APP is zero, then no
     application is linked in other than the default minimal
     application.

   After compiling, you can load the code using a tool like avarice or
   avrdude.  Debugging is possible under linux using avarice and a
   JTAGICE MK-II, or one of many other programmers that are available.

   @section other_flags Other compilation options

   There are a few other compilation options that are not global.

   - @ref RANDOM_DATA can be set to one to allow sending random data
     from sensor nodes, rather than actual measured data.  This is
     useful for demonstrating the networking software without having
     to connect real sensors to nodes.

   @defgroup radio Radio interface
   @defgroup mac The RUM network
   @defgroup timer_module Timer module
   @defgroup app Sample Application
   @defgroup serial Serial Port
   @defgroup sensors Sensor Application
 */

/*
        @defgroup mac_event MAC event handler
        @defgroup mac_route MAC routing functions
        @defgroup mac_scan MAC channel scanning functions
*/


#ifndef SYSX_H
#define SYSX_H

// Definitions of data structures used.
// Definitions of called and "callback" function names in the entire application.
// These functions must be defined in each layer - even if the function is empty.

#include "mac.h"

// Define some applications
/**
   Macro to define the sensor application. See @ref APP.

   @ingroup app
*/
#define SENSOR     1
#ifndef APP
/**
   The APP macro is used to define which application gets compiled in.
   If this macro is defined as zero, then only the basic demo
   application gets compiled.  If you set this macro to the value of
   @ref SENSOR, then the sensor app gets compiled in.

   @ingroup app
 */
#define APP SENSOR
#endif

/** 
   Enable or disable the sleep function by setting this macro to 1
   (on) or 0 (off).

   @ingroup app
*/
#ifndef SLEEP
#define SLEEP 0
#endif

// App functions
void appInit(void);

// App layer callbacks
void appScanConfirm(u8 success);
void appClearChanFound(u8 channel);
void appAssociateConfirm(u8 success);
void appPacketSendFailed(void);
void appResendPacket(void);
void appPacketSendSucceed(void);
void appTask(void);
void appPingReq(u16 addr);
void appPingRsp(u16 addr);
void appDataIndication(void);
void appChildAssociated(void);


// MAC layer functions
void macInit(u8 Channel);
void macStart(void);
void macScan(void);
u8 macIsScanning(void);
void macAssociate(u16 shortAddr, u8 channel);
void macDataRequest(u16 addr, u8 len, u8 * data);
void macPing(u8 pingType, u16 addr);

// Radio/HAL callback functions (called from driver)
void radioRxStartEvent(u8 const frame_length);
void radioTrxEndEvent(void);

#endif
