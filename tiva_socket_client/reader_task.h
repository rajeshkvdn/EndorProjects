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

typedef enum
{
    GET_PARAMS,
    SET_PARAMS,
    START_ATT,
    STOP_ATT
}
tReaderCmdType;

typedef struct
{
    tReaderCmdType rdCommandType;
    uint32_t cmdRespDelayMs;
    char *reqmsg;
}
tReaderEventReq;

typedef struct
{
    char *respmsg;
}
tReaderEventResp;

extern xQueueHandle g_QueModemResp;
extern xQueueHandle g_QueReaderReq;
extern xQueueHandle g_QueReaderResp;

uint32_t ReaderTaskInit(void);
static void ReaderTask(void *pvParameters);
void readerStartAttend(void);

#endif /* READER_TASK_H_ */
