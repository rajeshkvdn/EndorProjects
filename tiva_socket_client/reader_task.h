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
#include "timers.h"

#define DEVSN_LEN	14
#define SCHOOLID_LEN	4
#define DEVTIME_LEN	19
#define TAGID_LEN	8
#define TIME_LEN	8
#define TAG_COUNT_MAX	100
#define PACK_SIZE_MAX	18*TAG_COUNT_MAX+54


typedef enum
{
    GET_PARAMS,
    SET_PARAMS,
    START_ATT,
    STOP_ATT,
	CLR_BUF
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

typedef struct Tag_Info{
char tagid[TAGID_LEN];
char time[TIME_LEN];
}TagInfo;

typedef struct Att_Report{
char devsn[DEVSN_LEN];
char schoolid[SCHOOLID_LEN];
char devtime[DEVTIME_LEN];
TagInfo taglist[TAG_COUNT_MAX];
}AttReport;

extern xQueueHandle g_QueModemResp;
extern xQueueHandle g_QueReaderReq;
extern xQueueHandle g_QueReaderResp;
extern void vTimerCallback(xTimerHandle);
int updateTagList(char *idinfo);
int createAttndReport(void);



uint32_t ReaderTaskInit(void);
static void ReaderTask(void *pvParameters);
void readerStartAttend(void);

#endif /* READER_TASK_H_ */
