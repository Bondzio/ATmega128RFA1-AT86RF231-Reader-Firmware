/* /////////////////////////////////////////////////////////////////////////////////////////////////
//                     Copyright (c) Philips Semiconductors
//
//         All rights are reserved. Reproduction in whole or in part is
//        prohibited without the written consent of the copyright owner.
//    Philips reserves the right to make changes without notice at any time.
//   Philips makes no warranty, expressed, implied or statutory, including but
//   not limited to any implied warranty of merchantability or fitness for any
//  particular purpose, or that the use will not infringe any third party patent,
//   copyright or trademark. Philips must not be liable for any loss or damage
//                            arising from its use.
///////////////////////////////////////////////////////////////////////////////////////////////// */

#include <string.h>
#include <stdio.h>
#include <absacc.h>
#include <intrins.h>

#include "RegCtrl.h"
#include "reg52.h"
#include "main.h"
#include "Mifare.h"
#include "Rc522Reg.h"
#include "OpCtrl.h"
#include "ErrCode.h"

/*Initialize the processor and RC522*/
void SysInit(void)
{
	T2CON = 0x04;
    PCON = 0x80;
    SCON = 0x70;
    TMOD = 0x22;
	TH1   = BAUD_9600;  //set serial port baudrate to 9600bps
	TL1   = TH1;
	TR1   = 1;
	EA=0;           //disable all interrupt
 	Rc522Init();
}

//Just for test
void main(void)
{
    short status;
    unsigned char idata tmp[16],i,key[6];
    SysInit();
    memset(key,0xff,6);

    /* Request */
    status = Request(ISO14443_3_REQALL, tmp);
    if(status&&status != STATUS_COLLISION_ERROR)
      goto end;

    /*anticollision and select the card*/
    status = AnticollSelect(0, tmp);
    if(status&&status != STATUS_COLLISION_ERROR)
      goto end;
#if 0   /*test anti-collision function*/
    status = Request(ISO14443_3_REQALL, tmp);
    if(status&&status != STATUS_COLLISION_ERROR)
      goto end;

    /*anticollision and select the card*/
    status = AnticollSelect(0, tmp);
    if(status&&status != STATUS_COLLISION_ERROR)
      goto end;

    status = Request(ISO14443_3_REQALL, tmp);
    if(status&&status != STATUS_COLLISION_ERROR)
      goto end;

    /*anticollision and select the card*/
    status = AnticollSelect(0, tmp);
    if(status&&status != STATUS_COLLISION_ERROR)
      goto end;
#endif
    /*authenticaiton the key A of sector 1 (block 4~block 7)*/
    status = Authentication(MIFARE_AUTHENT_A,key, tmp, 6);
    if(status)
      goto end;

    /*if passed authenticaiton you can read the data in this sector*/
    status = Read(6, tmp);
    if(status)
      goto end;
    memset(tmp,0x33,16);

    /*write function test*/
    status = Write(6, tmp);
    if(status)
      goto end;
    memset(tmp,0,16);

    /*to check if the data has been writen successfully*/
    status = Read(6, tmp);
    if(status)
      goto end;

    /*Initialize a block value, then you can do increment and decrement operation*/
    memset(tmp,0xff,4);
    status = InitBlock(6,tmp);
    if(status)
      goto end;

    /*to check if the data has been initialized successfully*/
    status = Read(6, tmp);
    if(status)
      goto end;

    /*decrement operation test*/
    tmp[0]=2;
    tmp[1]=0;
    tmp[2]=0;
    tmp[3]=0;
    status = ValueOper(MIFARE_DECREMENT, 6, tmp, 6);
    if(status)
      goto end;

    /*check if the decrement operation is ok*/
    status = Read(6, tmp);
    if(status)
      goto end;

    /*Increment operation test*/
    tmp[0]=2;
    tmp[1]=0;
    tmp[2]=0;
    tmp[3]=0;
    status = ValueOper(MIFARE_INCREMENT, 6, tmp, 6);
    if(status)
      goto end;

    /*check if the increment operation is ok*/
    status = Read(6, tmp);
    if(status)
      goto end;
end:
    while(1);
}
