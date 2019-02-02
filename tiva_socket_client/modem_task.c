/*
 * modem_task.c
 *
 *  Created on: 14-Dec-2018
 *      Author: Rajesh
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
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

#define STACKSIZE_ModemTASK    1500

//*****************************************************************************
//
// Queue used to pass modem service commands and data from any other task
//
//*****************************************************************************
xQueueHandle g_QueModemReq;
xQueueHandle g_QueModemResp;


uint32_t
ModemTaskInit(void)
{

    g_QueModemReq  = xQueueCreate(16, sizeof(tModemEventReq));
    if(g_QueModemReq == 0)
    {
        return(1);
    }

    //
    // Create the Modem task.
    //
    if(xTaskCreate(ModemTask, (char *)"Modem",
                   STACKSIZE_ModemTASK, NULL, tskIDLE_PRIORITY +
                   PRIORITY_MODEM_TASK, NULL) != pdTRUE)
    {
        return(1);
    }

    //
    // Success.
    //
    return(0);
}


static void
ModemTask(void *pvParameters)
{
int len = 0;
char modemresp[256];
tModemEventReq modEventReq;

    while(1)
    {

        //
        // Block until a message is put on the g_QueSerial queue by the
        // interrupt handler.
        //
        if(xQueueReceive(g_QueModemReq, (void*) &modEventReq, (TickType_t)10) == pdTRUE)
        {
            switch(modEventReq.eCommandType)
            {
            case AT:
                cmd_test();
                break;
            case ATE0:
                cmd_echo_disable();
                break;
            case ATE1:
                cmd_echo_enable();
                break;
            case AT_CFUN:
                cmd_func_set();
                break;
            case AT_CUSD:
                cmd_serv_disable();
                break;
            case ATD:
                cmd_call_start("9446033603");
                break;
            case AT_CMGF:
                cmd_smsformat_set();
                break;
            case AT_CSMP:
                cmd_smsmode_set();
                break;
            case AT_CMGS:
                cmd_sms_send("8921339752", "Hello", 5);
                break;
            case AT_CGATT:
                cmd_gprs_stat();
                break;
            case AT_CSTT:
                cmd_apn_set();
                break;
            case AT_CIICR:
                cmd_wl_conn();
                vTaskDelay(1000);
                vTaskDelay(1000);
                break;
            case AT_CIFSR:
                cmd_ip_get();
                vTaskDelay(1000);
                vTaskDelay(1000);
                break;
            case AT_CIPSTART:
                cmd_conn_start();
                vTaskDelay(1000);
                vTaskDelay(1000);
                break;
            case AT_CIPSEND:
                cmd_send_gprsdata(modEventReq.payload);
                break;
            case AT_CIPCLOSE:
                cmd_conn_close();
                break;
            case AT_CIPSHUT:
                cmd_conn_shut();
                break;
            case AT_CGNSPWR:
                cmd_gnss_pwr();
                break;
            case AT_CGNSSEQ:
                cmd_nmea_seq();
                break;
            case AT_CGNSINF:
                cmd_cgns_info();
                break;
            default:
            }
            // Suspend task for a time specified for getting response from modem

            vTaskDelay(modEventReq.cmdRespDelayMs);

            //Read response received from modem, validate and pass it to the queue for calling task

            len = modemCmdGetResp(modemresp);

            UARTprintf("\n--------Response len : %d\n\n", len);
            UARTprintf("\n--------Response: %s\n\n", modemresp);

            if(modEventReq.eCommandType == AT_CGNSINF)
            {
                ModemCmdResp(AT_CGNSINF, modemresp);
            }

        }
        else
            vTaskDelay(1);

    }

}

void ModemCmdReq(tModemCmdType ctype, uint32_t delay, void *pparam)
{
    tModemEventReq tmodEvntReq;

    tmodEvntReq.eCommandType = ctype;
    tmodEvntReq.cmdRespDelayMs = delay;
    tmodEvntReq.payload = (char*)pparam;
    xQueueSend( g_QueModemReq, ( void * ) &tmodEvntReq, ( TickType_t ) 10 );

}

void ModemCmdResp(tModemCmdType ctype, char *msgresp)
{

    tModemEventResp tmodEvntResp;

    tmodEvntResp.eCommandType = ctype;
    strcpy(tmodEvntResp.mresp, msgresp);
    xQueueSend( g_QueModemResp, ( void * ) &tmodEvntResp, ( TickType_t ) 10 );

}

