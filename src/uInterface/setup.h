/*
 * setup.h
 *
 *  Created on: 14.11.2009
 *      Author: root
 */

#ifndef SETUP_H_
#define SETUP_H_

#define SYSTICK_INT_PRIORITY    0x80
#define ETHERNET_INT_PRIORITY   0xC0

void prvSetupHardware(void);

#endif /* SETUP_H_ */