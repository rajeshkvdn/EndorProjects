/*
 * ReaderTask.c
 *
 *  Created on: 22-Dec-2018
 *      Author: Rajesh
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/uart.h"
#include "utils/ringbuf.h"
#include "utils/uartstdio.h"
#include "config.h"
#include "priorities.h"
#include "serial.h"
#include "FreeRTOSConfig.h"
#include "gprsgps.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "modem_task.h"
#include "reader_task.h"

#define STACKSIZE_ReaderTASK    512


void GsmGprsInit(void);


static void
ReaderTask(void *pvParameters)
{
    tModemEventResp modEventResp;

    GsmGprsInit();
    while(1)
    {
        //
        // Block until a message is put on the g_QueSerial queue by the
        // interrupt handler.
        //
        if(xQueueReceive(g_QueModemResp, (void*) &modEventResp, (TickType_t)0) == pdTRUE)
        {
            if(modEventResp.eCommandType == AT_CGNSINF)
            {
                ModemCmdReq(AT_CGATT, 10, 0);
                ModemCmdReq(AT_CSTT, 10, 0);
                ModemCmdReq(AT_CIICR, 100, 0);
                ModemCmdReq(AT_CIFSR, 1000,0);
                ModemCmdReq(AT_CIPSTART, 1000, 0);
                ModemCmdReq(AT_CIPSEND, 1000, "Sending GPS data\n");
                ModemCmdReq(AT_CIPCLOSE, 100, 0);
                ModemCmdReq(AT_CIPSHUT, 1000, 0);
            }
        }
    }
    vTaskDelay(1000);
}


uint32_t
ReaderTaskInit(void)
{

    g_QueModemResp  = xQueueCreate(8, sizeof(tModemEventResp));
    if(g_QueModemResp == 0)
    {
        return(1);
    }

    //
    // Create the Modem task.
    //
    if(xTaskCreate(ReaderTask, (char *)"Reader",
                   STACKSIZE_ReaderTASK, NULL, tskIDLE_PRIORITY +
                   PRIORITY_READER_TASK, NULL) != pdTRUE)
    {
        return(1);
    }

    //
    // Success.
    //
    return(0);
}

void createReqQue(void)
{
    g_QueModemReq  = xQueueCreate(16, sizeof(tModemEventReq));
    if(g_QueModemReq == 0)
    {
        return(1);
    }
}

void GsmGprsInit(void)
{
    //Modem Initialization
    ModemCmdReq(AT, 10, 0);
    ModemCmdReq(ATE1, 10, 0);
    ModemCmdReq(AT_CFUN, 10, 0);
    ModemCmdReq(AT_CUSD, 10, 0);

#if TST_VOICE_CALL
    //Voice call
    ModemCmdReq(ATD, 10, 0);
#endif
#if TST_SMS_ALERT
    ModemCmdReq(AT_CMGF, 10, 0);
    ModemCmdReq(AT_CSMP, 10, 0);
    ModemCmdReq(AT_CMGS, 10, 0);
#endif
#if TST_GPRS_UPLOAD
    ModemCmdReq(AT_CGATT, 10, 0);
    ModemCmdReq(AT_CSTT, 10, 0);
    ModemCmdReq(AT_CIICR, 100, 0);
    ModemCmdReq(AT_CIFSR, 1000, 0);
    ModemCmdReq(AT_CIPSTART, 1000, 0);
    ModemCmdReq(AT_CIPSEND, 1000, "GPRS Upload OK\n");
    ModemCmdReq(AT_CIPCLOSE, 100, 0);
    ModemCmdReq(AT_CIPSHUT, 1000, 0);
#endif
#if TST_GPS_TRACK
    ModemCmdReq(AT_CGNSPWR, 100, 0);
    ModemCmdReq(AT_CGNSSEQ, 100, 0);
    ModemCmdReq(AT_CGNSINF, 1000, 0);
#endif


}

