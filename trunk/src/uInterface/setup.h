/**
 * \addtogroup Configuration
 * @{
 *
 * \file setup.h
 * \author Anziner, Hahn
 * \brief global config file for the project
 *
 *
 */

#ifndef SETUP_H_
#define SETUP_H_

#define SYSTICK_INT_PRIORITY    0x80
#define ETHERNET_INT_PRIORITY   0xC0

#define ENABLE_LOG	0
#define ENABLE_GRAPHIC	1

#define ENABLE_SNTP			0
#define ENABLE_DNS			0
#define	ENABLE_NET_BIOS 	0

//#define DEBUG_MEMORY 1
#define DEBUG_GRAPHIC 		0
#define DEBUG_HTTPC			0
#define DEBUG_SSI 			0
#define DEBUG_SSI_PARAMS	0
#define DEBUG_LOG 			0
#define DEBUG_CGI 			0
#define DEBUG_COM			0

void prvSetupHardware(void);

#endif /* SETUP_H_ */