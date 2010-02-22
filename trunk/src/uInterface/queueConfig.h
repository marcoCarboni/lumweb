/**
 * \addtogroup Configuration
 * @{
 *
 * \file queueConfig.h
 * \author Anziner, Hahn
 * \brief Configfile for queues
 *
 *
 */

#ifndef QUEUECONFIG_H_
#define QUEUECONFIG_H_

#include "FreeRTOS.h"
#include "queue.h"


#define COM_QUEUE_SIZE 		6
#define HTTPD_QUEUE_SIZE 	6
//#define GRAPH_QUEUE_SIZE 	6

xQueueHandle xComQueue;
xQueueHandle xHttpdQueue;
//xQueueHandle xGraphQueue;


#endif /* QUEUECONFIG_H_ */