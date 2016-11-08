/* /////////////////////////////////////////////////////////////////////////////////////////////////
//                     Copyright (c) Philips Semiconductors
//
//                       (C)PHILIPS Electronics
//         All rights are reserved. Reproduction in whole or in part is
//        prohibited without the written consent of the copyright owner.
//    Philips reserves the right to make changes without notice at any time.
//   Philips makes no warranty, expressed, implied or statutory, including but
//   not limited to any implied warranty of merchantability or fitness for any
//  particular purpose, or that the use will not infringe any third party patent,
//   copyright or trademark. Philips must not be liable for any loss or damage
//                            arising from its use.
///////////////////////////////////////////////////////////////////////////////////////////////// */

/*! \file ErrCode.h
 *
 * Project: Object Oriented Library Framework.
 *
 * Workfile: ErrCode.h
 * $Author: mha $
 * $Revision: 1.1 $
 * $Date: Fri Jun 30 14:33:59 2006 $
 *
 * $Author: mha $
 * $Revision: 1.1 $
 * $Date: Fri Jun 30 14:33:59 2006 $
 *
 * Comment:
 *  None.
 *
 *
 * History:
 *  GK: Generated 4. Sept. 2002
 *
*/
/*! \skipline STATUS_H__INCLUDED */

#ifndef STATUS_H__INCLUDED                                                                          /*  */
#define STATUS_H__INCLUDED                                                                          /*  */

/* S U C C E S S                                                                                            */
/*! \name Success
    \ingroup error */
/*@{*/
#define STATUS_SUCCESS                  (0x0000)            /*!< Returned in case of no error when there
                                                                    isn't any more appropriate code.        */
/*@}*/


/* C O M M U N I C A T I O N                                                                                */
/*! \name Communication Errors/Status Values
    \ingroup error */
/*@{*/
#define STATUS_IO_TIMEOUT               (0x0001)  /*!< No reply received, e.g. PICC removal.    */
#define STATUS_CRC_ERROR                (0x0002)  /*!< Wrong CRC detected by RC or library.     */
#define STATUS_PARITY_ERROR             (0x0003)  /*!< Parity error detected by RC or library.  */
#define STATUS_BITCOUNT_ERROR           (0x0004)  /*!< Typically, the RC reports such an error. */
#define STATUS_FRAMING_ERROR            (0x0005)  /*!< Invalid frame format.                    */
#define STATUS_COLLISION_ERROR          (0x0006)  /*!< Typically, the RC repors such an error.  */
#define STATUS_BUFFER_TOO_SMALL         (0x0007)  /*!< Communication buffer size insufficient.  */
#define STATUS_ACCESS_DENIED            (0x0008)  /*!< Access has not been granted (readonly?). */
#define STATUS_BUFFER_OVERFLOW          (0x0009)  /*!< Attempt to write beyond the end of a
                                                          buffer.                                  */
#define STATUS_PROTOCOL_ERROR           (0x000B)  /*!< Mifare start bit wrong, buffer length
                                                          error.                                   */
#define STATUS_ERROR_NY_IMPLEMENTED     (0x000C)  /*!< Feature not yet implemented.             */
#define STATUS_FIFO_WRITE_ERROR         (0x000D)  /*!< Error caused because of interface conflict
                                                          during write access to FIFO.             */
#define STATUS_USERBUFFER_FULL          (0x000E)  /*!< The user buffer is full, the calling
                                                          application/routine gets the chance to
                                                          save user buffer data and start over.    */
/*@}*/


/* I N T E R F A C E (Device as well as function parameters) errors:                               */
/*! \name Interface Errors/Status Values
    \ingroup error */
/*@{*/
#define STATUS_INVALID_PARAMETER        (0x0101)  /*!< Parameter is invalid (range, format).    */
#define STATUS_UNSUPPORTED_PARAMETER    (0x0102)  /*!< Parameter value/format is correct but not
                                                          supported in the current configuration.  */
#define STATUS_UNSUPPORTED_COMMAND      (0x0103)  /*!< The device does not support the command. */
#define STATUS_INTERFACE_ERROR          (0x0104)  /*!< Host-peripheral interface error.         */
#define STATUS_INVALID_FORMAT           (0x0105)  /*!< The data format does not match the spec. */
#define STATUS_INTERFACE_NOT_ENABLED    (0x0106)  /*!< This interface is currently(!) not
                                                          supported (e.g. function ptr. to NULL).  */
/*@}*/



/* M F  errors:                                                                                             */
/*! \name Mifare Errors/Status Values
    \ingroup error */
/*@{*/
#define STATUS_AUTHENT_ERROR            (0x0201)  /*!< Authentication failure (e.g. key
                                                          mismatch).                                         */
#define STATUS_ACK_SUPPOSED             (0x0202)  /*!< Single byte or nibble received, CRC error
                                                          detected, possibly MF (N)ACK response.            */
/*@}*/


/* I S O 1 4 4 4 3 . 4 Level specific errors:                                                               */
/*! \name ISO 14443-4 Errors/Status Values
    \ingroup error */
/*@{*/
#define STATUS_BLOCKNR_NOT_EQUAL        (0x0301)  /*!< Frame OK, but Blocknumber mismatch.               */
/*@}*/


/* N F C  Errors and Stati:                                                                                 */
/*! \name NFC Errors
    \ingroup error */
/*@{*/
#define STATUS_TARGET_DEADLOCKED        (0x0401)  /*!< Target has not sent any data, but RF
                                                          (generated by the Target) is still switched on.   */
#define STATUS_TARGET_SET_TOX           (0x0402)  /*!< Target has sent Timeout Extension Request.        */
#define STATUS_TARGET_RESET_TOX         (0x0403)  /*!< Reset timeout-value after Timeout Extension.      */
/*@}*/


/* I S O 1 4 4 4 3 . 3 Level specific errors:                                                     */
/*! \name ISO 14443-3 Errors/Status Values
    \ingroup error */
/*@{*/
#define STATUS_WRONG_UID_CHECKBYTE      (0x0501)  /*!< UID check byte is wrong.                 */
#define STATUS_WRONG_HALT_FORMAT        (0x0502)  /*!< HALT Format error.                       */
/*@}*/


/*  I D  M A N A G E R  specific errors:                                                                    */
/*! \name ID-Manager Errors/Status Values
    \ingroup error */
/*@{*/
#define STATUS_ID_ALREADY_IN_USE        (0x0601)  /*!< ID cannot be assigned because it is already used. */
#define STATUS_INSTANCE_ALREADY_IN_USE  (0x0602)  /*!< INSTANCE cannot be assigned because it is already used. */
#define STATUS_ID_NOT_IN_USE            (0x0603)  /*!< Specified ID is not in use.                       */
#define STATUS_NO_ID_AVAILABLE          (0x0604)  /*!< No ID is available, all are occupied.             */
/*@}*/


/* O T H E R   E R R O R S :                                                                       */
/*! \name Other Errors/Status Values
    \ingroup error */
/*@{*/
#define STATUS_OTHER_ERROR              (0x7E01)  /*!< Unspecified, error, non-categorised.     */
#define STATUS_INSUFFICIENT_RESOURCES   (0x7E02)  /*!< The system runs low of resources!        */
#define STATUS_INVALID_DEVICE_STATE     (0x7E03)  /*!< The (sub-)system is in a state which
                                                          does not allow the operation.            */
#define STATUS_RC522_TEMP_ERROR        (0x7E04)  /*!< Temperature error indicated by RC522 HW.*/
/*@}*/


#endif                                               /* STATUS_H__INCLUDED                    */

/* EOF */
