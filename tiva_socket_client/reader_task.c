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
#include "rfid_cmd_data.h"

#define STACKSIZE_ReaderTASK    1024

xQueueHandle g_QueReaderReq;
xQueueHandle g_QueReaderResp;

tReaderEventReq rdEventReq;
tReaderEventResp rdEventResp;

void GsmGprsInit(void);
void readerSetParams(void);
void readerStartAttend(void);
void readerStopAttend(void);
void readerClearBuf(void);

char rdcmd_setpar[700];
char rdcmd_statt[700];
//char studentinfo[1800];

static int tagcount;
AttReport studentlist;
char attdpack[PACK_SIZE_MAX];


uint8_t timeExpired = 0;
char *msgp;

void vTimerCallback(xTimerHandle pxTimer)
{
timeExpired = 1;
}

static void
ReaderTask(void *pvParameters)
{
    tModemEventResp modEventResp;

    GsmGprsInit();
    readerStopAttend();
    readerSetParams();
    readerStartAttend();
//    memset(studentinfo, 0, sizeof(studentinfo));

    while(1)
    {
    	if(timeExpired == 1)
    	{

    		timeExpired = 0;
    		readerStopAttend();
    		readerClearBuf();
    		readerStartAttend();
    		UARTprintf("\nTIME OUT!!!!!!!!!!!");
			createAttndReport();
			tagcount = 0;

    		/*Get GPS Info*/
    		ModemCmdReq(AT_CGNSINF, 1000, 0);
        	if(xQueueReceive(g_QueModemResp, (void*) &modEventResp, (TickType_t)0) == pdTRUE)
        	{
        		if(modEventResp.eCommandType == AT_CGNSINF)
        		{
        	/*Make final pay load*/
        			//strcat(studentinfo, modEventResp.mresp);

        	/*Send pay load via GPRS*/
//        			ModemCmdReq(AT_CGATT, 10, 0);
//        			ModemCmdReq(AT_CSTT, 10, 0);
//        			ModemCmdReq(AT_CIICR, 100, 0);
//        			ModemCmdReq(AT_CIFSR, 1000,0);
//        			ModemCmdReq(AT_CIPSTART, 1000, 0);
        			//ModemCmdReq(AT_CIPSEND, 1000, modEventResp.mresp);
        			//ModemCmdReq(AT_CIPSEND, 1000, rdEventResp.respmsg);
//        			ModemCmdReq(AT_CIPSEND, 1000, studentinfo);
        			ModemCmdReq(AT_CIPSEND, 1000, attdpack);
//        			ModemCmdReq(AT_CIPCLOSE, 100, 0);
//        			ModemCmdReq(AT_CIPSHUT, 1000, 0);
        		}
        	}

//    		memset(studentinfo, 0, sizeof(studentinfo));
    	}

    	if(xQueueReceive(g_QueReaderResp, (void*) &rdEventResp, (TickType_t)10) == pdTRUE)
    	{

    		UARTprintf("\n------------------Reader Attendance Response--------\n");
    		//UARTprintf("%s", rdEventResp.respmsg);


    		msgp = strstr(rdEventResp.respmsg,"deviceJson");

    		updateTagList(msgp);

    		UARTprintf("\n======================= Tag Count : %d\n", tagcount);

//    		strcat(studentinfo, msgp);
    	}
    	//
    	// Block until a message is put on the g_QueSerial queue by the
    	// interrupt handler.
    	//

    }
}


uint32_t
ReaderTaskInit(void)
{
//Message queue for Modem response string
    g_QueModemResp  = xQueueCreate(8, sizeof(tModemEventResp));
    if(g_QueModemResp == 0)
    {
        return(1);
    }
//Message Queue for Reader Request
    g_QueReaderReq  = xQueueCreate(8, sizeof(tReaderEventReq));
    if(g_QueReaderReq == 0)
    {
        return(1);
    }

//Message Queue for Reader Response
        g_QueReaderResp  = xQueueCreate(8, sizeof(tReaderEventResp));
        if(g_QueReaderResp == 0)
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
//    ModemCmdReq(AT_CIPCLOSE, 100, 0);
//    ModemCmdReq(AT_CIPSHUT, 1000, 0);
#endif
#if TST_GPS_TRACK
    ModemCmdReq(AT_CGNSPWR, 100, 0);
    ModemCmdReq(AT_CGNSSEQ, 100, 0);
    ModemCmdReq(AT_CGNSINF, 1000, 0);
#endif

}

void readerSetParams(void)
{
    memset(rdcmd_setpar, 0, sizeof(rdcmd_setpar));
    rdEventReq.rdCommandType = SET_PARAMS;
    //rdEventReq.reqmsg = jsonCmdSetParams();
    strcpy(rdcmd_setpar,jsonCmdSetParams());
    UARTprintf("\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
    UARTprintf("%s", rdcmd_setpar);
    rdEventReq.reqmsg = rdcmd_setpar;
    xQueueSend( g_QueReaderReq, ( void * ) &rdEventReq, ( TickType_t ) 10 );

}

void readerStartAttend(void)
{
    memset(rdcmd_statt, 0, sizeof(rdcmd_statt));
    rdEventReq.rdCommandType = START_ATT;
//    rdEventReq.reqmsg = jsonCmdStartAttd();
    strcpy(rdcmd_statt,jsonCmdStartAttd());
    rdEventReq.reqmsg = rdcmd_statt;
    xQueueSend( g_QueReaderReq, ( void * ) &rdEventReq, ( TickType_t ) 10 );

}

void readerStopAttend(void)
{
    memset(rdcmd_statt, 0, sizeof(rdcmd_statt));
    rdEventReq.rdCommandType = STOP_ATT;
//    rdEventReq.reqmsg = jsonCmdStartAttd();
    strcpy(rdcmd_statt,jsonCmdStopAttd());
    rdEventReq.reqmsg = rdcmd_statt;
    xQueueSend( g_QueReaderReq, ( void * ) &rdEventReq, ( TickType_t ) 10 );

}

void readerClearBuf(void)
{
    memset(rdcmd_statt, 0, sizeof(rdcmd_statt));
    rdEventReq.rdCommandType = CLR_BUF;
//    rdEventReq.reqmsg = jsonCmdStartAttd();
    strcpy(rdcmd_statt,jsonCmdClearBuff());
    rdEventReq.reqmsg = rdcmd_statt;
    xQueueSend( g_QueReaderReq, ( void * ) &rdEventReq, ( TickType_t ) 10 );

}

/*
update tag id time stamp and other information to the global taginfo structure
param : idinfo - pointer to the received RFID data in the format
	deviceJson={\"DevSN\":\"A30C18082221E3\",\"SchoolID\":\"0000\",\"DevTime\":\"2018-10-03 13:03:00\",\"AttenceID\":[{\"ID\":\"19DC4B1D\",\"Time\":\"12:56:35\"},{\"ID\":\"2F400976\",\"Time\":\"12:56:35\"},{\"ID\":\"2F42FD86\",\"Time\":\"12:56:35\"},{\"ID\":\"2F2F6AA6\",\"Time\":\"12:56:35\"},{\"ID\":\"2F41A216\",\"Time\":\"12:56:35\"},{\"ID\":\"2F4425C6\",\"Time\":\"12:56:36\"},{\"ID\":\"19DC4B1D\",\"Time\":\"12:56:40\"},{\"ID\":\"2F41A216\",\"Time\":\"12:56:40\"},{\"ID\":\"2F2F6AA6\",\"Time\":\"12:56:40\"},{\"ID\":\"2F400976\",\"Time\":\"12:56:40\"},{\"ID\":\"2F42FD86\",\"Time\":\"12:56:40\"},{\"ID\":\"2F4425C6\",\"Time\":\"12:56:41\"},{\"ID\":\"2F42FD86\",\"Time\":\"12:56:45\"},{\"ID\":\"2F400976\",\"Time\":\"12:56:45\"},{\"ID\":\"19DC4B1D\",\"Time\":\"12:56:45\"}]}
*/

int updateTagList(char *idinfo)
{
	char *infoptr;
	int i;

	infoptr = strstr(idinfo, "DevSN");
	if(infoptr == NULL )
		return -1;
	memcpy(studentlist.devsn, infoptr+8, DEVSN_LEN);

	infoptr = strstr(idinfo, "SchoolID");
	if(infoptr == NULL )
		return -1;
	memcpy(studentlist.schoolid, infoptr+11, SCHOOLID_LEN);

	infoptr = strstr(idinfo, "DevTime");
	if(infoptr == NULL )
		return -1;
	memcpy(studentlist.devtime, infoptr+10, DEVTIME_LEN);


	infoptr = strstr(idinfo, "AttenceID");
	if(infoptr == NULL )
		return -1;
	infoptr = infoptr+12;

	for(i = tagcount; ; i++)
	{
		infoptr = strstr(infoptr, "ID");
		if(infoptr == NULL )
			break;
		memcpy(studentlist.taglist[i].tagid, infoptr+5, TAGID_LEN);

		infoptr = strstr(infoptr, "Time");
		if(infoptr == NULL )
			break;
		memcpy(studentlist.taglist[i].time, infoptr+7, TIME_LEN);

	}
	tagcount = i;

	return tagcount;
}


/*
Packing parsed data to upload
Create a Attndence report of following format. This is to reduce payload size
$<SeqNumber>#<TagCount>#A30C18082221E3#0000#2018-09-28 11:18:05##19DC4B1D@11:15:11#2F42FD86@11:15:11#2F41A216@11:15:11#2F4425C6@11:15:11#2F2F6AA6@11:15:11#2F400976@11:15:11$
*/


int createAttndReport(void)
{
	int index=0, len, i;
	char buff[20];

	memset(attdpack, 0, sizeof(attdpack));


	memcpy(attdpack+index, "$", 1);
	index = index+1;

	//Reserve space of 5 digit sequence number to be added before sendind packet
	memcpy(attdpack+index, "00000", 5);
	index = index+5;

	memcpy(attdpack+index, "#", 1);
	index = index+1;

	len = sprintf(buff,"%d", tagcount);

	memcpy(attdpack+index, buff, len);
	index = index+len;

	memcpy(attdpack+index, "#", 1);
	index = index+1;

	memcpy(attdpack+index, studentlist.devsn, DEVSN_LEN);
	index = index+DEVSN_LEN;

	memcpy(attdpack+index, "#", 1);
	index = index+1;

	memcpy(attdpack+index, studentlist.schoolid, SCHOOLID_LEN);
	index = index+SCHOOLID_LEN;

	memcpy(attdpack+index, "#", 1);
	index = index+1;

	memcpy(attdpack+index, studentlist.devtime, DEVTIME_LEN);
	index = index+DEVTIME_LEN;

	memcpy(attdpack+index, "##", 2);
	index = index+2;

	for(i=0 ; i<tagcount ; i++)
	{
		memcpy(attdpack+index, studentlist.taglist[i].tagid, TAGID_LEN);
		index = index+TAGID_LEN;

		memcpy(attdpack+index, "@", 1);
		index = index+1;

		memcpy(attdpack+index, studentlist.taglist[i].time, TIME_LEN);
		index = index+TIME_LEN;

		memcpy(attdpack+index, "#", 1);
		index = index+1;
	}
	return index;
}


