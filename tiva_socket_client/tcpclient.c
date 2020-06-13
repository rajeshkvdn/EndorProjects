/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: rkv <rajeshkv@endorautomation.com>
 *
 */


#include "lwip/opt.h"


#if LWIP_NETCONN

#include "lwip/sys.h"
#include "lwip/api.h"
#include "utils/uartstdio.h"
#include "string.h"
#include "modem_task.h"
#include "reader_task.h"


#define TCPECHO_THREAD_PRIO  ( tskIDLE_PRIORITY + 3 )

char rqmsg[1024];
char resp[1200]={};

tReaderEventResp rdEvntResp;
/*-----------------------------------------------------------------------------------*/
static void tcpecho_thread(void *arg)
{
    struct netconn *conn, *newconn;
    err_t err;
    struct netbuf *buf;
    void *data;
    u16_t len, leni;
    ip_addr_t local_ipaddr, remote_ipaddr;

    tReaderEventReq readEventReq;

    IP4_ADDR( &local_ipaddr,192,168,1,200);
    IP4_ADDR( &remote_ipaddr,192,168,1,250);

    LWIP_UNUSED_ARG(arg);

    /* Create a new connection identifier. */
    conn = netconn_new(NETCONN_TCP);

    if (conn != NULL)
    {
        /* Bind connection to well known port number 7. */
        //    err = netconn_bind(conn, IP_ADDR_ANY, 7);
        err = netconn_bind(conn, &local_ipaddr, 6000);

        if (err == ERR_OK)
        {
            /* Tell connection to go into listening mode. */
            err = netconn_connect ( conn, &remote_ipaddr, 60000 );

            UARTprintf(" netconn_connect  %d\n", err);
            /* Grab new connection. */

            while (1)
            {
                if(xQueueReceive(g_QueReaderReq, (void*) &readEventReq, (TickType_t)10) == pdTRUE)
                {
                    memset(rqmsg, 0, sizeof(rqmsg));
                    switch(readEventReq.rdCommandType)
                    {
                    case GET_PARAMS:
                        strcpy(rqmsg, readEventReq.reqmsg);
                        break;
                    case SET_PARAMS:
                        strcpy(rqmsg, readEventReq.reqmsg);
                        break;
                    case START_ATT:
                        strcpy(rqmsg, readEventReq.reqmsg);
                        break;
                    case STOP_ATT:
                        strcpy(rqmsg, readEventReq.reqmsg);
                        break;
                    default:
                    }
                    len = strlen(rqmsg);
                    err = netconn_write(conn, rqmsg, len, NETCONN_COPY);
                    if(err == ERR_OK)
                        UARTprintf(" Send OK\n");
                    else
                        UARTprintf(" Send Failed %d\n", err);

//                    UARTprintf(" Waiting for Response\n");
                }
                //while (err = (netconn_recv(conn, &buf)));
                err = (netconn_recv(conn, &buf));
                if(err == ERR_OK)
                {
                    netbuf_data(buf, &data, &len);
                    memcpy(resp, (char*)data, len);
                    resp[len]='\0';

//                    UARTprintf(" Reader Response:\n %s\n", resp);

                        rdEvntResp.respmsg = resp;
                        xQueueSend( g_QueReaderResp, ( void * ) &rdEvntResp, ( TickType_t ) 10 );
                    while (netbuf_next(buf) >= 0);
                    netbuf_delete(buf);

                }

                vTaskDelay(500);
            } /*while()*/
        }
        else
        {
            UARTprintf(" can not bind TCP netconn\n");
        }
    }
    else
    {
        UARTprintf("can not create TCP netconn");
    }
}
/*-----------------------------------------------------------------------------------*/

void tcpecho_init(void)
{
  sys_thread_new("tcpecho_thread", tcpecho_thread, NULL, DEFAULT_THREAD_STACKSIZE, TCPECHO_THREAD_PRIO);
}
/*-----------------------------------------------------------------------------------*/

#endif /* LWIP_NETCONN */
