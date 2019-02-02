/*
 * modem_task.h
 *
 *  Created on: 14-Dec-2018
 *      Author: Rajesh
 */

#ifndef MODEM_TASK_H_
#define MODEM_TASK_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#define TST_VOICE_CALL 0
#define TST_SMS_ALERT 0
#define TST_GPRS_UPLOAD 1
#define TST_GPS_TRACK 1
//
//Strtucture deffinitions of Modem Communication Task
//
//*****************************************************************************
//
//! Possible modem command types.
//
//*****************************************************************************
typedef enum
{
    AT,
    ATE0,
    ATE1,
    AT_CFUN,
    AT_CGNSPWR,
    AT_CGNSSEQ,
    AT_CGNSINF,
    AT_CGATT,
    AT_CSTT,
    AT_CIICR,
    AT_CIFSR,
    AT_CIPSTART,
    AT_CIPSEND,
    AT_CIPCLOSE,
    AT_CIPSHUT,
    AT_CMGF,
    AT_CSMP,
    AT_CUSD,
    AT_CMGS,
    ATD,
    ATH
}
tModemCmdType;

typedef struct
{
    tModemCmdType eCommandType;
    uint32_t cmdRespDelayMs;
    char *payload;
}
tModemEventReq;

typedef struct
{
    tModemCmdType eCommandType;
    char mresp[1200];
}
tModemEventResp;

extern xQueueHandle g_QueModemReq;

uint32_t ModemTaskInit(void);
static void ModemTask(void *pvParameters);
void ModemCmdReq(tModemCmdType ctype, uint32_t delay, void *pparam);
void ModemCmdResp(tModemCmdType ctype, char *msgresp);

#endif /* MODEM_TASK_H_ */
