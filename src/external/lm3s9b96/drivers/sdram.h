//*****************************************************************************
//
// sdram.h - Functions related to initialisation and management of SDRAM.
//
// Copyright (c) 2009 Luminary Micro, Inc.  All rights reserved.
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
// This is part of revision 4905 of the DK-LM3S9B96 Firmware Package.
//
//*****************************************************************************

#ifndef _SDRAM_H_
#define _SDRAM_H_
#define INCLUDE_BGET_STATS 1

//*****************************************************************************
//
// Public function prototypes.
//
//*****************************************************************************
extern tBoolean SDRAMInit(unsigned long ulEPIDivider, unsigned long ulConfig,
                          unsigned long ulRefresh);
extern void *SDRAMAlloc(unsigned long ulSize);
extern void SDRAMFree(void *pvBlock);
extern unsigned long SDRAMMaxFree(unsigned long *pulTotalFree);
#endif // _SDRAM_H_
