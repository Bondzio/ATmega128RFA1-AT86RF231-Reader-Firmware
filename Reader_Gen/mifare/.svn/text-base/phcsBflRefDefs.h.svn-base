/* /////////////////////////////////////////////////////////////////////////////////////////////////
//                     Copyright (c) Philips Semiconductors
//
//                       (C)PHILIPS Electronics N.V.2004
//         All rights are reserved. Reproduction in whole or in part is
//        prohibited without the written consent of the copyright owner.
//    Philips reserves the right to make changes without notice at any time.
//   Philips makes no warranty, expressed, implied or statutory, including but
//   not limited to any implied warranty of merchantability or fitness for any
//  particular purpose, or that the use will not infringe any third party patent,
//   copyright or trademark. Philips must not be liable for any loss or damage
//                            arising from its use.
///////////////////////////////////////////////////////////////////////////////////////////////// */

/*! 
 * \if cond_generate_filedocu 
 * \file phcsBflRefDefs.h
 *
 * Project: Object Oriented Library Framework Reference Definitions (Incl. Platform Definitions).
 *
 * Workfile: phcsBflRefDefs.h
 * $Author: nxp13887 $
 * $Revision: 1.5.1.1 $
 * $Date: Tue Jun  9 16:46:18 2009 $
 *
 * Comment:
 *  None.
 *
 * History:
 *  GK:  Generated 4. Sept. 2002
 *  MHa: Migrated to MoReUse September 2005
 * \endif
 *
 */

#ifndef PHCSBFLREFDEFS_H
#define PHCSBFLREFDEFS_H

#include <phcsTypeDefs.h>


/* NULL: */
#ifndef NULL       /* */
    #define NULL 0 /* */
#endif


/*! \name Unreferenced parameters warning disable macro
 */
/*@{*/
/*  \ingroup gen_macro */
/*!
 *  Enable compilation at increased warning level by pretending the usage of otherwise
 *  unreferenced parameters.
 */
#define PHCS_BFL_DISABLE_WARNING_UNREF(P) { (P) = (P); } /* */

/*!
 *  Enable compilation at increased warning level by pretending the usage of otherwise
 *  unreferenced parameters.
 */
#define PHCS_BFL_ADD_COMPCODE(stat, code) { \
    if(((stat) != PH_ERR_BFL_SUCCESS) && (((stat) & PH_ERR_BFL_COMP_MASK) == 0)) \
        {status = (stat) | (code);} \
    else \
        {status = (stat);} } /* */
/*@}*/


 /*
 *  Platform depending definitions for memory location of variables: We distinguish between
 *   - BUFFERs   : Location of storage, usually mid-speed but max. RAM consumption,
 *   - COUNTERs  : loop counters or frequently accessed variables, fast access,
 *   - REMINDERs : variables holding status values or serving flow control purposes.
 *
 * If RCL is used on a microcontroller, an own REFDEF.H file must be generated where the
 * RCL_MEMLOC_... definition must get according values. Default definitions are empty.
 *
 * Assignment Example: XDATA for PHCS_BFL_LARGE_EXT_RAM,
 *                     IDATA for PHCS_BFL_FAST_INT_RAM,
 *                     EDATA for PHCS_BFL_EXTD_INT_RAM.
 */
 
/*! \name Internal variables memory location specifiers
 */
/*@{*/
/*  \ingroup gen_macro */

/*! 
 *  \brief To be defined to e.g. XDATA when used on a microcontroller.
 */
#define PHCS_BFL_LARGE_EXT_RAM     /* */

/*! 
 *  \brief To be defined to e.g. IDATA when used on a microcontroller.
 */
#define PHCS_BFL_FAST_INT_RAM      /* */

/*! 
 *  \brief To be defined to e.g. EDATA when used on a microcontroller.
 */
#define PHCS_BFL_EXTD_INT_RAM  /* */

/*! 
 *  \brief Space used for storage (e.g. buffers). 
 */
#define PHCS_BFL_MEMLOC_BUF     PHCS_BFL_LARGE_EXT_RAM    /* */

/*! 
 *  \brief Space used for fast and frequent access e.g. counters, loop variables).
 */
#define PHCS_BFL_MEMLOC_COUNT    PHCS_BFL_FAST_INT_RAM     /* */      

/*! 
 *   \brief Space used for control variables, these could be e.g. status values.
 */
#define PHCS_BFL_MEMLOC_REM   PHCS_BFL_EXTD_INT_RAM /* */   
/*@}*/  

#endif /*  PHCSBFLREFDEFS_H */
