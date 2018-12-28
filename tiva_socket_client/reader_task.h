/*
 * reader_task.h
 *
 *  Created on: 22-Dec-2018
 *      Author: Rajesh
 */

#ifndef READER_TASK_H_
#define READER_TASK_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"



extern xQueueHandle g_QueModemResp;

uint32_t ReaderTaskInit(void);
static void ReaderTask(void *pvParameters);

#endif /* READER_TASK_H_ */
