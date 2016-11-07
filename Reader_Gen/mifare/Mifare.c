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

/*! \file Mifare.c
 *
 * Project: Mifare reader with RC522
 *
 * Workfile: Mifare.c
 * $Author: mha $
 * $Revision: 1.1 $
 * $Date: Fri Jun 30 14:33:59 2006 $
 *
 * Comment:
 *  All the ISO14443-3 protocol and mifare command set are all implemented here.
 *  All the founctions in this file is totally independent of hardware.
 *  The source can be ported to other platforms very easily.
 *
 *  The interrupt pin of the reader IC is not conntected and no interrupt needed.
 *  All protocol relevant timing constraints are generated
 *  by the internal timer of the reader module.
 *
 *  Therefore the function M522PcdCmd is very important for understanding
 *  of the communication between reader and card.
 *
 *
 * History:
 *
 *
*/
#include <string.h>

#include "RegCtrl.h"
#include "Rc522Reg.h"
#include "OpCtrl.h"
#include "ErrCode.h"
#include "Mifare.h"


typedef struct
         {
            unsigned char  cmd;                 //!< command code
            char           status;              //!< communication status
            unsigned char  nBytesSent;          //!< how many bytes already sent
            unsigned char  nBytesToSend;        //!< how many bytes to send
            unsigned char  nBytesReceived;      //!< how many bytes received
            unsigned short nBitsReceived;       //!< how many bits received
            unsigned char  collPos;             //collision position
         } MfCmdInfo;

#define ResetInfo(info)    \
            info.cmd            = 0;\
            info.status         = STATUS_SUCCESS;\
            info.nBytesSent     = 0;\
            info.nBytesToSend   = 0;\
            info.nBytesReceived = 0;\
            info.nBitsReceived  = 0;\
            info.collPos        = 0;

//volatile MfCmdInfo MInfo;
//volatile MfCmdInfo *MpIsrInfo = 0;
MfCmdInfo MInfo;
MfCmdInfo *MpIsrInfo = 0;

unsigned char  SerBuffer[20];

/* ***********************************************
Function:       SetPowerDown
Description:
     set the rc522 enter or exit power down mode
Parameter:
     ucFlag     0   --  exit power down mode
                !0  --  enter power down mode
Return:
     short      status of implement
************************************************* */
void SetPowerDown(unsigned char ucFlag)
{
    unsigned char RegVal;
/*
    Note: The bit Power Down can not be set when the SoftReset command has been activated.
*/
    if(ucFlag)
    {
        RegVal = RcGetReg(JREG_COMMAND);  //enter power down mode
        RegVal |= 0x10;
        RcSetReg(JREG_COMMAND, RegVal);
    }
    else
    {
        RegVal = RcGetReg(JREG_COMMAND);  //disable power down mode
        RegVal &= (~0x10);
        RcSetReg(JREG_COMMAND, RegVal);
    }
}

/* ***********************************************
Function:       SetTimeOut
Description:
     Adjusts the timeout in 100us steps
Parameter:
     uiMicroSeconds   the time to set the timer(100us as a step)
Return:
     short      status of implement
************************************************* */
short SetTimeOut(unsigned int uiMicroSeconds)
{
    unsigned int RegVal;
    unsigned char TmpVal;
    RegVal = uiMicroSeconds / 100;
    /*
    NOTE: The supported hardware range is bigger, since the prescaler here
          is always set to 100 us.
    */
    if(RegVal >= 0xfff)
    {
        return STATUS_INVALID_PARAMETER;
    }
	// set bit
    RcModifyReg(JREG_TMODE, 1, JBIT_TAUTO);  // 2Ah

    RcSetReg(JREG_TPRESCALER, 0xa6);	// 2Bh

    TmpVal = RcGetReg(JREG_TMODE);
    TmpVal &= 0xf0;
    TmpVal |= 0x02;
    RcSetReg(JREG_TMODE, TmpVal);//82
    
    RcSetReg(JREG_TRELOADLO, ((unsigned char)(RegVal&0xff))); // 2Dh
    RcSetReg(JREG_TRELOADHI, ((unsigned char)((RegVal>>8)&0xff))); // 2Ch
    return STATUS_SUCCESS;
}


/* ***********************************************
Function:       ChangeRc522BaudRate
Description:
     Changes the serial speed of the RC522.
     Note that the speed of the host interface (UART on PC) has to be also set to the
     appropriate one.
Parameter:
     baudrate   new baudrate for rc522
Return:
     short      status of implement
************************************************* */
short ChangeRc522BaudRate(unsigned long baudrate)
{
    short   status = STATUS_SUCCESS;
    unsigned char setRegVal;
    switch(baudrate)
    {
        case 9600:
            setRegVal = 0xEB;
            break;

        case 14400:
            setRegVal = 0xDA;
            break;

        case 19200:
            setRegVal = 0xCB;
            break;

        case 38400:
            setRegVal = 0xAB;
            break;

        case 57600:
            setRegVal = 0x9A;
            break;

        case 115200:
            setRegVal = 0x7A;
            break;

        case 128000:
            setRegVal = 0x74;
            break;

        default:
            status = STATUS_INVALID_PARAMETER;
            break;
    }

	/* Set the appropriate value */
	if (status == STATUS_SUCCESS)
		RcSetReg(JREG_SERIALSPEED,setRegVal);
	/* Now the RC522 is set to the new speed*/
    return status;
}

/* ***********************************************
Function:       Rc523Init
Description:
     initialize rc523 as a mifare reader
Parameter:
     NONE
Return:
     short      status of implement
************************************************* */
void Rc522Init(void)
{
    unsigned char RegVal;

    RcSetReg(JREG_COMMAND, 0x0f); /*reset the RC522*/

    RcSetReg(JREG_TXASK, 0x40); /*force to 100% ASK*/

    /* disable Crypto1 bit*/
    RcModifyReg(JREG_STATUS2, 0, JBIT_CRYPTO1ON);

    /* do not touch bits: InvMod in register TxMode */
    RegVal = RcGetReg(JREG_TXMODE);
    RegVal = (unsigned char)(RegVal & JBIT_INVMOD);
    RegVal = (unsigned char)(RegVal | JBIT_CRCEN | (RCO_VAL_RF106K << RC522_SPEED_SHL_VALUE));
    /* TxCRCEn = 1; TxSpeed = x; InvMod, TXMix = 0; TxFraming = 0 */
    RcSetReg(JREG_TXMODE, RegVal);
    
	/* do not touch bits: RxNoErr in register RxMode */
    RegVal = RcGetReg(JREG_RXMODE);
    RegVal = (unsigned char)(RegVal & JBIT_RXNOERR);
    RegVal = (unsigned char)(RegVal | JBIT_CRCEN | (RCO_VAL_RF106K << RC522_SPEED_SHL_VALUE));
     /* RxCRCEn = 1; RxSpeed = x; RxNoErr, RxMultiple = 0; TxFraming = 0 */
    RcSetReg(JREG_RXMODE, RegVal);
	
    /* ADDIQ = 10b; FixIQ = 1; RFU = 0; TauRcv = 11b; TauSync = 01b */
    RcSetReg(JREG_DEMOD, 0x6D);

    /* RxGain = 4*/
    RcSetReg(JREG_RFCFG, 0x48);

    /* do settings common for all functions */
    RcSetReg(JREG_RXTRESHOLD, 0x55);    /* MinLevel = 5; CollLevel = 5 */

    RcSetReg(JREG_MODWIDTH, 0x26);      /* Modwidth = 0x26 */
    RcSetReg(JREG_GSN, 0xF0 | 0x04);     /* CWGsN = 0xF; ModGsN = 0x4 */

    /* Set the timer to auto mode, 5ms using operation control commands before HF is switched on to
     * guarantee Iso14443-3 compliance of Polling procedure
	 */
    SetTimeOut(5000);

    /* Activate the field  */
    RcModifyReg(JREG_TXCONTROL, 1, JBIT_TX2RFEN | JBIT_TX1RFEN);

    /* start timer manually to check the initial waiting time */
    RcModifyReg(JREG_CONTROL, 1, JBIT_TSTARTNOW);

    /*
	 * After switching on the timer wait until the timer interrupt occurs, so that
     * the field is on and the 5ms delay have been passed.
	 */
    do {
        RegVal = RcGetReg(JREG_COMMIRQ);
    }
    while(!(RegVal & JBIT_TIMERI));


    /* Clear the status flag afterwards */
    RcSetReg(JREG_COMMIRQ, JBIT_TIMERI);

    /*
	 * Reset timer 1 ms using operation control commands (AutoMode and Prescaler are the same)
	 * set reload value
	 */
    SetTimeOut(5000);

    RcSetReg(JREG_WATERLEVEL, 0x1A);
    RcSetReg(JREG_TXSEL, 0x10);
    RcSetReg(JREG_RXSEL, 0x84);



    /* Activate receiver for communication
       The RcvOff bit and the PowerDown bit are cleared, the command is not changed. */
    RcSetReg(JREG_COMMAND, JCMD_IDLE);

	/* Set timeout for REQA, ANTICOLL, SELECT to 200us */
	SetTimeOut(2000);
}

/* ***********************************************
Function:       M522PcdCmd
Description:
     implement a command
Parameter:
     cmd            command code
     ExchangeBuf    saved the data will be send to card and the data response from the card
     info           some information for the command
Return:
     short      status of implement
************************************************* */
short  M522PcdCmd(unsigned char cmd,
                 unsigned char *ExchangeBuf,
                 MfCmdInfo  *info)
{
    short          status    = STATUS_SUCCESS;

    unsigned char   commIrqEn   = 0;
    unsigned char   divIrqEn    = 0;
    unsigned char   waitForComm = JBIT_ERRI | JBIT_TXI;
    unsigned char   waitForDiv  = 0;
    unsigned char   doReceive   = 0;
    unsigned char   i;
    unsigned char   getRegVal,setRegVal;

    unsigned char  nbytes, nbits;
    unsigned int counter;

    /*remove all Interrupt request flags that are used during function,
    keep all other like they are*/
    RcSetReg(JREG_COMMIRQ, waitForComm);
    RcSetReg(JREG_DIVIRQ, waitForDiv);
    RcSetReg(JREG_FIFOLEVEL, JBIT_FLUSHBUFFER);

    /*disable command or set to transceiver*/
    getRegVal = RcGetReg(JREG_COMMAND);
    if(cmd == JCMD_TRANSCEIVE)
    {
        /*re-init the transceiver command*/
        setRegVal = (getRegVal & ~JMASK_COMMAND) | JCMD_TRANSCEIVE;
        RcSetReg(JREG_COMMAND, setRegVal);//0c
    }
    else
    {
        /*clear current command*/
        setRegVal = (getRegVal & ~JMASK_COMMAND);
        RcSetReg(JREG_COMMAND, setRegVal);
    }
    MpIsrInfo = info;
    switch(cmd)
    {
       case JCMD_IDLE:         /* values are 00, so return immediately after all bytes written to FIFO */
            waitForComm = 0;
            waitForDiv  = 0;
            break;
        case JCMD_CALCCRC:      /* values are 00, so return immediately after all bytes written to FIFO */
            waitForComm = 0;
            waitForDiv  = 0;
            break;
        case JCMD_TRANSMIT:
            commIrqEn = JBIT_TXI | JBIT_TIMERI;
            waitForComm = JBIT_TXI;
            break;
        case JCMD_RECEIVE:
            commIrqEn = JBIT_RXI | JBIT_TIMERI | JBIT_ERRI;
            waitForComm = JBIT_RXI | JBIT_TIMERI | JBIT_ERRI;
            doReceive = 1;
            break;
        case JCMD_TRANSCEIVE:
            commIrqEn = JBIT_RXI | JBIT_TIMERI | JBIT_ERRI;
            waitForComm = JBIT_RXI | JBIT_TIMERI | JBIT_ERRI;
            doReceive = 1;
            break;
        case JCMD_AUTHENT:
            commIrqEn = JBIT_IDLEI | JBIT_TIMERI | JBIT_ERRI;
            waitForComm = JBIT_IDLEI | JBIT_TIMERI | JBIT_ERRI;
            break;
        case JCMD_SOFTRESET:    /* values are 0x00 for IrqEn and for waitFor, nothing to do */
            waitForComm = 0;
            waitForDiv  = 0;
            break;
        default:
            status = STATUS_UNSUPPORTED_COMMAND;
    }
    if(status == STATUS_SUCCESS)
    {
        /* activate necessary communication Irq's */
        getRegVal = RcGetReg(JREG_COMMIEN);
        RcSetReg(JREG_COMMIEN, getRegVal | commIrqEn);

        /* activate necessary other Irq's */
        getRegVal = RcGetReg(JREG_DIVIEN);
        RcSetReg(JREG_DIVIEN, getRegVal | divIrqEn);

        /*write data to FIFO*/
        for(i=0; i<MpIsrInfo->nBytesToSend; i++)
        {
            RcSetReg(JREG_FIFODATA, ExchangeBuf[i]);
        }

        /*do seperate action if command to be executed is transceive*/
        if(cmd == JCMD_TRANSCEIVE)
        {
            /*TRx is always an endless loop, Initiator and Target must set STARTSEND.*/
            RcModifyReg(JREG_BITFRAMING, 1, JBIT_STARTSEND);
        }
        else
        {
            getRegVal = RcGetReg(JREG_COMMAND);
            RcSetReg(JREG_COMMAND, (getRegVal & ~JMASK_COMMAND) | cmd);
        }

        /*polling mode*/
        getRegVal = 0;
        setRegVal = 0;
        counter = 0; /*Just for debug*/
		
        while(!(waitForComm ? (waitForComm & setRegVal) : 1) ||
              !(waitForDiv ? (waitForDiv & getRegVal) :1))
        {
            setRegVal = RcGetReg(JREG_COMMIRQ);
            getRegVal = RcGetReg(JREG_DIVIRQ);
            counter ++;
            if(counter > 0x0100)
                break;
        }
        /*store IRQ bits for clearance afterwards*/
        waitForComm = (unsigned char)(waitForComm & setRegVal);
        waitForDiv  = (unsigned char)(waitForDiv & getRegVal);

        /*set status to Timer Interrupt occurrence*/
        if (setRegVal & JBIT_TIMERI)
        {
            status = STATUS_IO_TIMEOUT;
        }
    }

    /*disable all interrupt sources*/
    RcModifyReg(JREG_COMMIEN, 0, commIrqEn);

    RcModifyReg(JREG_DIVIEN, 0, divIrqEn);

    if(doReceive && (status == STATUS_SUCCESS))
    {
        /*read number of bytes received (used for error check and correct transaction*/
        MpIsrInfo->nBytesReceived = RcGetReg(JREG_FIFOLEVEL);
        nbytes = MpIsrInfo->nBytesReceived;
        getRegVal = RcGetReg(JREG_CONTROL);
        MpIsrInfo->nBitsReceived = (unsigned char)(getRegVal & 0x07);
        nbits = MpIsrInfo->nBitsReceived;

        getRegVal = RcGetReg(JREG_ERROR);
        /*set status information if error occurred*/
        if(getRegVal)
        {
            if(getRegVal & JBIT_COLLERR)
                status = STATUS_COLLISION_ERROR;         /* Collision Error */
            else if(getRegVal & JBIT_PARITYERR)
                status = STATUS_PARITY_ERROR;            /* Parity Error */

            if(getRegVal & JBIT_PROTERR)
                status = STATUS_PROTOCOL_ERROR;          /* Protocoll Error */
            else if(getRegVal & JBIT_BUFFEROVFL)
                status = STATUS_BUFFER_OVERFLOW;         /* BufferOverflow Error */
            else if(getRegVal & JBIT_CRCERR)
            {   /* CRC Error */
                if(MpIsrInfo->nBytesReceived == 0x01 &&
                    (MpIsrInfo->nBitsReceived == 0x04 ||
                     MpIsrInfo->nBitsReceived == 0x00))
                {   /* CRC Error and only one byte received might be a Mifare (N)ACK */
                    ExchangeBuf[0] = RcGetReg(JREG_FIFODATA);
                    MpIsrInfo->nBytesReceived = 1;
                    status = STATUS_ACK_SUPPOSED;        /* (N)ACK supposed */
                }
                else
                    status = STATUS_CRC_ERROR;           /* CRC Error */
            }
            else if(getRegVal & JBIT_TEMPERR)
                status = STATUS_RC522_TEMP_ERROR;       /* Temperature Error */
            if(getRegVal & JBIT_WRERR)
                status = STATUS_FIFO_WRITE_ERROR;        /* Error Writing to FIFO */
            if(status == STATUS_SUCCESS)
                status = STATUS_ERROR_NY_IMPLEMENTED;    /* Error not yet implemented, shall never occur! */

            /* if an error occured, clear error register before IRQ register */
            RcSetReg(JREG_ERROR, 0);
        }

        /*read data from FIFO and set response parameter*/
        if(status != STATUS_ACK_SUPPOSED)
        {
            for(i=0; i<MpIsrInfo->nBytesReceived; i++)
            {
                ExchangeBuf[i] = RcGetReg(JREG_FIFODATA);
            }
            /*in case of incomplete last byte reduce number of complete bytes by 1*/
            if(MpIsrInfo->nBitsReceived && MpIsrInfo->nBytesReceived)
                MpIsrInfo->nBytesReceived --;
        }
    }
    RcSetReg(JREG_COMMIRQ, waitForComm);
    RcSetReg(JREG_DIVIRQ, waitForDiv);
    RcSetReg(JREG_FIFOLEVEL, JBIT_FLUSHBUFFER);
    RcSetReg(JREG_COMMIRQ, JBIT_TIMERI);
    RcSetReg(JREG_BITFRAMING, 0);
    return status;
}

/* ***********************************************
Function:       Request
Description:
     REQA, request to see if have a ISO14443A card in the field
Parameter:
     req_code   command code(ISO14443_3_REQALL or ISO14443_3_REQIDL)
     atq        the buffer to save the answer to request from the card
Return:
     short      status of implement
************************************************* */
short Request(unsigned char req_code, unsigned char *atq)
{
   char  status = STATUS_SUCCESS;

   /* *********** initialize **************** */
   RcModifyReg(JREG_STATUS2, 0, JBIT_CRYPTO1ON);  /* disable Crypto if activated before */
   RcSetReg(JREG_COLL, JBIT_VALUESAFTERCOLL);  //active values after coll
   RcModifyReg(JREG_TXMODE, 0, JBIT_CRCEN);    //disable TxCRC
   RcModifyReg(JREG_RXMODE, 0, JBIT_CRCEN);    //disable RxCRC
   RcSetReg(JREG_BITFRAMING, REQUEST_BITS);

   /* set necessary parameters for transmission */
   ResetInfo(MInfo);
   SerBuffer[0] = req_code;
   MInfo.nBytesToSend   = 1;

   /* Set timeout for REQA, ANTICOLL, SELECT*/
	SetTimeOut(400);

   status = M522PcdCmd(JCMD_TRANSCEIVE,
                      SerBuffer,
                      &MInfo);
   if(status == STATUS_SUCCESS || status == STATUS_COLLISION_ERROR)
   {
       if(MInfo.nBytesReceived != ATQA_LENGTH || MInfo.nBitsReceived != 0x00)
       {
           status = STATUS_PROTOCOL_ERROR;
       }
       else
       {
           memcpy(atq,SerBuffer,2);
       }
   }
   else
   {   /* reset atqa parameter */
       atq[0] = 0x00;
       atq[1] = 0x00;
   }
   //RcSetReg(JREG_BITFRAMING, 0);
   return status;
}

/* ***********************************************
Function:       CascAnticoll
Description:
     Functions to split anticollission and select internally.
     NOTE: this founction is used internal only, and cannot call by application program
Parameter:
     sel_code   command code
     bitcount   the bit counter of known UID
     snr        the UID have known
Return:
     short      status of implement
************************************************* */
short CascAnticoll(unsigned char sel_code,
                   unsigned char bitcount,
                   unsigned char *snr)
{
    short status = STATUS_SUCCESS;

    unsigned char   i;
    unsigned char   complete = 0; /* signs end of anticollission loop */
    unsigned char   rbits    = 0; /* number of total received bits */
    unsigned char   nbits    = 0; /* */
    unsigned char   nbytes   = 0; /* */
    unsigned char   byteOffset;   /* stores offset for ID copy if uncomplete last byte was sent */

    /* initialise relvant bytes in internal buffer */
    for(i=2;i<7;i++)
        SerBuffer[i] = 0x00;

    /* disable TxCRC and RxCRC */
    RcModifyReg(JREG_TXMODE, 0, JBIT_CRCEN);
    RcModifyReg(JREG_RXMODE, 0, JBIT_CRCEN);

    /* activate deletion of bits after coll */
    RcSetReg(JREG_COLL, 0);

    /* init parameters for anticollision */
    while(!complete && (status == STATUS_SUCCESS))
    {
         /* if there is a communication problem on the RF interface, bcnt
            could be larger than 32 - folowing loops will be defective. */
        if(bitcount > SINGLE_UID_LENGTH)
        {
            status = STATUS_INVALID_PARAMETER;
            continue;
        }

        /* prepare data length */
        nbits = (unsigned char)(bitcount % BITS_PER_BYTE);
        nbytes = (unsigned char)(bitcount / BITS_PER_BYTE);
        if(nbits)
            nbytes++;

        /* prepare data buffer */
        SerBuffer[0] = sel_code;
        SerBuffer[1] = (unsigned char)(NVB_MIN_PARAMETER + ((bitcount / BITS_PER_BYTE) << UPPER_NIBBLE_SHIFT) + nbits);
        for(i=0;i<nbytes;i++)
            SerBuffer[2+i] = snr[i];   /* copy serial number to tranmit buffer */

        /* set TxLastBits and RxAlign to number of bits sent */
        RcSetReg(JREG_BITFRAMING, (unsigned char)((nbits << UPPER_NIBBLE_SHIFT) | nbits));

        /* prepare data for common transceive */
        ResetInfo(MInfo);
        MInfo.nBytesToSend   = (unsigned char)(nbytes + 2);

        SetTimeOut(10000);
        status = M522PcdCmd(JCMD_TRANSCEIVE, SerBuffer, &MInfo);

        if(status == STATUS_COLLISION_ERROR || status == STATUS_SUCCESS)
        {
            /* store number of received data bits and bytes internaly */
            rbits = (unsigned char)(MInfo.nBitsReceived + (MInfo.nBytesReceived << 3) - nbits);

            if((rbits + bitcount) > COMPLETE_UID_BITS)
            {
                status = STATUS_BITCOUNT_ERROR;
                continue;
            }

            /* increment number of bytes received if also some bits received */
            if(MInfo.nBitsReceived)
                MInfo.nBytesReceived++;

            /* reset offset for data copying */
            byteOffset = 0;
            /* if number of bits sent are not 0, write first received byte in last of sent */
            if(nbits)
            {   /* last byte transmitted and first byte received are the same */
                snr[nbytes - 1] |= SerBuffer[0];
                byteOffset++;
            }

            for(i=0;i<(4-nbytes);i++)
                snr[nbytes + i] = SerBuffer[i + byteOffset];

            if(status == STATUS_COLLISION_ERROR)
            {
                /* calculate new bitcount value */
                bitcount = (unsigned char)(bitcount + rbits);
                status = STATUS_SUCCESS;
            } else
            {
                if((snr[0] ^ snr[1] ^ snr[2] ^ snr[3]) != SerBuffer[i + byteOffset])
                {
                    status = STATUS_WRONG_UID_CHECKBYTE;
                    continue;
                }
                complete=1;
            }
        }
    }

    /* clear RxAlign and TxLastbits */
    RcSetReg(JREG_BITFRAMING, 0);

    /* activate values after coll */
    RcSetReg(JREG_COLL, JBIT_VALUESAFTERCOLL);
    return status;
}

/* ***********************************************
Function:       Select
Description:
     selecte a card to response the following command
     NOTE: this founction is used internal only, and cannot call by application program
Parameter:
     sel_code   command code
     snr        buffer to store the card UID
     sak        the byte to save the ACK from card
Return:
     short      status of implement
************************************************* */
short Select(unsigned char sel_code, unsigned char *snr, unsigned char *sak)
{
    short status = STATUS_SUCCESS;
    /* define local variables */
    unsigned char i;
    /* activate CRC */
    RcModifyReg(JREG_TXMODE, 1, JBIT_CRCEN);
    RcModifyReg(JREG_RXMODE, 1, JBIT_CRCEN);

    /* prepare data stream */
    SerBuffer[0] = sel_code;   /* command code */
    SerBuffer[1] = NVB_MAX_PARAMETER;       /* parameter */
    for(i=0;i<4;i++)
        SerBuffer[2+i] = snr[i];   /* serial numbner bytes 1 to 4 */
    SerBuffer[6] = (unsigned char)(snr[0] ^ snr[1] ^ snr[2] ^ snr[3]);   /* serial number check byte */

    /* prepare data for common transceive */
    ResetInfo(MInfo);
    MInfo.nBytesToSend   = 0x07;
    SetTimeOut(2000);
    status = M522PcdCmd(JCMD_TRANSCEIVE, SerBuffer, &MInfo);

    if(status == STATUS_SUCCESS)
    {
        if(MInfo.nBytesReceived == SAK_LENGTH && MInfo.nBitsReceived == 0)
            *sak = SerBuffer[0];
        else
            status = STATUS_BITCOUNT_ERROR;
    }
    return status;
}

/* ************************************************
Function:       AnticollSelect
Description:
     Anticollision loop and selecte a card to operate
Parameter:
     bcnt       bit counter of known UID
     snr        buffer to store the card UID
	 sct		store selected card type
Return:
     short      status of implement
************************************************* */
short AnticollSelect(unsigned char bcnt, unsigned char *snr, unsigned char *sct)
{
    unsigned char i;
    short status=STATUS_SUCCESS;
    unsigned char length, casc_code, length_in,sak,tmpSnr[12];
    length_in = bcnt;
    /* do loop for max. cascade level */
    for(i=0;i<MAX_CASCADE_LEVELS;i++)
    {
        if(length_in)
        {
            if(length_in > SINGLE_UID_LENGTH)
            {
                length = SINGLE_UID_LENGTH;
                length_in -= SINGLE_UID_LENGTH;
            }
            else
            {
                length = length_in;
                length_in = 0;
            }
        }
        else
        {
            length = 0;
        }

        switch(i)
        {
            case 1:  casc_code = SELECT_CASCADE_LEVEL_2;
                     memcpy(snr, tmpSnr+1,3);
                     break;
            case 2:  casc_code = SELECT_CASCADE_LEVEL_3;
                     break;
            default: casc_code = SELECT_CASCADE_LEVEL_1;
                     break;
        }

        if(length != SINGLE_UID_LENGTH && status == STATUS_SUCCESS)
        /* do anticollission with selected level */
        status = CascAnticoll(casc_code,
                              length,
                              tmpSnr + i * 4);


        if(status == STATUS_SUCCESS)
        {
            /* select 1st cascade level uid */
            status = Select(casc_code, tmpSnr + i * 4, &sak);

            /* check if further cascade level is used */
            if(status == STATUS_SUCCESS)
            {
                /* increase number of received bits in parameter */
                bcnt = (unsigned char)(SINGLE_UID_LENGTH * (i + 1)); //the actually length of the UID, you can return it.

                /* check if cascade bit is set */
                if(!(sak & CASCADE_BIT))
                {
                    break;
                }
            }
        }
        else
        {
            break;
        }
    }
    switch(i)
    {
        case 1:  memcpy(snr+3, tmpSnr+4,4);     //copy UID to snr buffer without CT(0x88)
                 break;
        case 2:  memcpy(snr+6, tmpSnr+4,4);
                 break;
        default: memcpy(snr, tmpSnr,4);
                 break;
    }
	
	*sct = sak;
    return status;
}

/* ************************************************
Function:       HaltA
Description:
     halt the current selected card
Parameter:
     NONE
Return:
     short      status of implement
************************************************* */
short HaltA(void)
{
    short  status = STATUS_SUCCESS;
    /* initialise data buffer */
    SerBuffer[0] = HALTA_CMD;
    SerBuffer[1] = HALTA_PARAM;

    ResetInfo(MInfo);
    MInfo.nBytesToSend   = HALTA_CMD_LENGTH;
    SetTimeOut(1000);
    status = M522PcdCmd(JCMD_TRANSCEIVE, SerBuffer, &MInfo);

    if(status == STATUS_IO_TIMEOUT)
        status = STATUS_SUCCESS;
    return status;
}

/* ************************************************
Function:       Authentication
Description:
     authentication the password for a sector of mifare card
Parameter:
     auth_mode  specify key A or key B -- MIFARE_AUTHENT_A or MIFARE_AUTHENT_A
     key        the buffer stored the key(6 bytes)
     snr        the buffer stored the selected card's UID
     addr       the block address of a sector
Return:
     short      status of implement
************************************************* */
short Authentication(unsigned char auth_mode,
                     unsigned char *key,
                     unsigned char *snr,
                     unsigned char addr)
{
    short status;
    unsigned char RegVal;

    ResetInfo(MInfo);

    SerBuffer[0] = auth_mode;      //key A or key B
    SerBuffer[1] = addr;           //address to authentication
    memcpy(SerBuffer+2,key,6);     //6 bytes key
    memcpy(SerBuffer+8,snr,4);     //4 bytes UID
    MInfo.nBytesToSend = 12;       //length
    SetTimeOut(2000);
    status = M522PcdCmd(JCMD_AUTHENT, SerBuffer, &MInfo);
    if(status == STATUS_SUCCESS)
    {
        RegVal = RcGetReg(JREG_STATUS2);
        if((RegVal & 0x0f) != 0x08)
            status = STATUS_AUTHENT_ERROR;
    }
    return status;
}

/* ************************************************
Function:       Read
Description:
     read 16 bytes data from a block
Parameter:
     addr       the address of the block
     _data      the buffer to save the 16 bytes data
Return:
     short      status of implement
************************************************* */
short Read(unsigned char addr, unsigned char *_data)
{
   short status = STATUS_SUCCESS;

   ResetInfo(MInfo);
   SerBuffer[0] = MIFARE_READ;
   SerBuffer[1] = addr;
   MInfo.nBytesToSend   = 2;
   SetTimeOut(10000);
   status = M522PcdCmd(JCMD_TRANSCEIVE,
                       SerBuffer,
                       &MInfo);

   if (status != STATUS_SUCCESS)
   {
      if (status != STATUS_IO_TIMEOUT )     // no timeout occured
      {
         if (MInfo.nBitsReceived == 4)
         {
             SerBuffer[0] &= 0x0f;
             if ((SerBuffer[0] & 0x0a) == 0)
             {
                status = STATUS_AUTHENT_ERROR;
             }
             else
             {
                status = STATUS_INVALID_FORMAT;
             }
          }
      }
      memset(_data,0,16);
   }
   else   // Response Processing
   {
      if (MInfo.nBytesReceived != 16)
      {
         status = STATUS_ACCESS_DENIED;
         memset(_data,0,16);
      }
      else
      {
         memcpy(_data,SerBuffer,16);
      }
   }
   return status;
}

/* ************************************************
Function:       Write
Description:
     write 16 bytes data to a block
Parameter:
     addr       the address of the block
     _data      the data to write
Return:
     short      status of implement
************************************************* */
short Write( unsigned char addr, unsigned char *_data)
{
    short status = STATUS_SUCCESS;
    ResetInfo(MInfo);
    SerBuffer[0] = MIFARE_WRITE;
    SerBuffer[1] = addr;
    MInfo.nBytesToSend   = 2;
    SetTimeOut(20000);
    status = M522PcdCmd(JCMD_TRANSCEIVE,
                        SerBuffer,
                        &MInfo);

    if (status != STATUS_IO_TIMEOUT)
    {
       if (MInfo.nBitsReceived != 4)
       {
          status = STATUS_BITCOUNT_ERROR;
       }
       else
       {
          SerBuffer[0] &= 0x0f;
          if ((SerBuffer[0] & 0x0a) == 0)
          {
             status = STATUS_AUTHENT_ERROR;
          }
          else
          {
             if (SerBuffer[0] == 0x0a)
             {
                status = STATUS_SUCCESS;
             }
             else
             {
                status = STATUS_INVALID_FORMAT;
             }
          }
       }
    }

    if ( status == STATUS_SUCCESS)
    {

       SetTimeOut(5000);

       ResetInfo(MInfo);
       memcpy(SerBuffer,_data,16);
       MInfo.nBytesToSend   = 16;
       status = M522PcdCmd(JCMD_TRANSCEIVE,
                           SerBuffer,
                           &MInfo);

       if (status & 0x80)
       {
          status = STATUS_IO_TIMEOUT;
       }
       else
       {
          if (MInfo.nBitsReceived != 4)
          {
             status = STATUS_BITCOUNT_ERROR;
          }
          else
          {
             SerBuffer[0] &= 0x0f;
             if ((SerBuffer[0] & 0x0a) == 0)
             {
                status = STATUS_ACCESS_DENIED;
             }
             else
             {
                if (SerBuffer[0] == 0x0a)
                {
                   status = STATUS_SUCCESS;
                }
                else
                {
                   status = STATUS_INVALID_FORMAT;
                }
             }
          }
       }
    }
  return status;
}

/* ************************************************
Function:       ValueOper
Description:
     block value operation function, increment or decrement the block value
     and transfer to a block
Parameter:
     OperMode   MIFARE_INCREMENT or MIFARE_DECREMENT
     addr       the address of the block
     value      the value to be increment or decrement
     trans_addr the address to save the resulet of increment or decrement
Return:
     short      status of implement
************************************************* */
short ValueOper(unsigned char OperMode,
                    unsigned char addr,
                    unsigned char *value,
                    unsigned char trans_addr)
{
   short status = STATUS_SUCCESS;
   ResetInfo(MInfo);
   SerBuffer[0] = OperMode;
   SerBuffer[1] = addr;
   MInfo.nBytesToSend   = 2;
   SetTimeOut(20000);
   status = M522PcdCmd(JCMD_TRANSCEIVE,
                       SerBuffer,
                       &MInfo);

   if (status != STATUS_IO_TIMEOUT)
   {
        if (MInfo.nBitsReceived != 4)
        {
           status = STATUS_BITCOUNT_ERROR;
        }
        else
        {
           SerBuffer[0] &= 0x0f;
           switch(SerBuffer[0])
           {
              case 0x00:
                 status = STATUS_AUTHENT_ERROR;
                 break;
              case 0x0a:
                 status = STATUS_SUCCESS;
                 break;
              case 0x01:
                 status = STATUS_INVALID_FORMAT;
                 break;
              default:
                 status = STATUS_OTHER_ERROR;
                 break;
           }
        }
     }

     if ( status == STATUS_SUCCESS)
     {
        SetTimeOut(10000);
        ResetInfo(MInfo);
        memcpy(SerBuffer,value,4);
        MInfo.nBytesToSend   = 4;
        status = M522PcdCmd(JCMD_TRANSCEIVE,
                            SerBuffer,
                            &MInfo);

        if (status == STATUS_IO_TIMEOUT||(status == MIFARE_DECREMENT && OperMode == MIFARE_DECREMENT))
        {
            status = STATUS_SUCCESS;
        }
        else
        {
            status = STATUS_OTHER_ERROR;
        }
     }
     if ( status == STATUS_SUCCESS)
     {
        ResetInfo(MInfo);
        SerBuffer[0] = MIFARE_TRANSFER;
        SerBuffer[1] = trans_addr;
        MInfo.nBytesToSend   = 2;
        status = M522PcdCmd(JCMD_TRANSCEIVE,
                            SerBuffer,
                            &MInfo);

        if (status & MIFARE_ACK_MASK)
        {
            status = STATUS_SUCCESS;
        }
        else
        {
            status = STATUS_OTHER_ERROR;
        }
     }
   return status;
}
 
/* ************************************************
Function:       InitBlock
Description:
     initialize a block value
Parameter:
     addr       the address of the block
     value      the value to be initialized, 4 bytes buffer
Return:
     short      status of implement
************************************************* */
short InitBlock(unsigned char addr,unsigned char *value)
{
	unsigned char tmp[16],i;
	short status = STATUS_SUCCESS;
	for(i=0;i<4;i++)
	{
		tmp[i]=value[i];
		tmp[i+4]=255-value[i];
		tmp[i+8]=value[i];
	}
	tmp[12]=addr;
	tmp[13]=255-addr;
	tmp[14]=tmp[12];
	tmp[15]=tmp[13];
	status=Write(addr,tmp);
	return status;
}


short readCardID(unsigned char cardType, unsigned char *buffer)
{
	unsigned char zone = 4;

	if(cardType == MIFARE_1K)
	{
		zone = 1;
	}
	
	//Read CardID from User Zone
	return(Read(zone, buffer));
	
}