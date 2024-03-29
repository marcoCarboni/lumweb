//*****************************************************************************
//
// io.h - Prototypes for I/O routines
//
// Copyright (c) 2007-2009 Luminary Micro, Inc.  All rights reserved.
// Software License Agreement
// 
// Luminary Micro, Inc. (LMI) is supplying this software for use solely and
// exclusively on LMI's microcontroller products.
// 
// The software is owned by LMI and/or its suppliers, and is protected under
// applicable copyright laws.  All rights are reserved.  You may not combine
// this software with "viral" open-source software in order to form a larger
// program.  Any use in violation of the foregoing restrictions may subject
// the user to criminal sanctions under applicable laws, as well as to civil
// liability for the breach of the terms and conditions of this license.
// 
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
// CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 4053 of the EK-LM3S8962 Firmware Package.
//
//*****************************************************************************

/**
 * \addtogroup CGIandSSI
 * @{
 *
 * \author Anziner, Hahn
 * \brief Prototypes for I/O routines
 *
 */

#ifndef __IO_H__
#define __IO_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "ethernet/lwipopts.h"
#include "setup.h"

#ifdef INCLUDE_HTTPD_SSI

/**
 The number of individual SSI tags that the HTTPD server can expect to
 find in our configuration pages.
 */
#include "taglib/taglib.h" // Definition of Strings
#endif

#include "ethernet/httpd/cgi/ssiparams.h"

//char **paramsSet = NULL, **valuesSet = NULL;
int paramValueLen; /// number of params/values set last time - 1

void io_init(void);

int io_get_value_from_comtask(char* id);

#ifdef __cplusplus
}
#endif

#endif // __IO_H__
//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
