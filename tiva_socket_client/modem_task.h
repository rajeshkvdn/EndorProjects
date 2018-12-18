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
    uint8_t mdata2;
}
tModemEvent;

extern xQueueHandle g_QueModemReq;

uint32_t ModemTaskInit(void);
static void ModemTask(void *pvParameters);

#endif /* MODEM_TASK_H_ */