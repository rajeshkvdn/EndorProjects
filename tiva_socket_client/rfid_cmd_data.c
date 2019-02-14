/*
 * rfid_cmd_data.c
 *
 *  Created on: 05-Jan-2019
 *      Author: Rajesh
 */




/*
File includes RFID reader commads and parsed data routines


*/

#include <stdio.h>
#include <string.h>
#include "rfid_cmd_data.h"

char cmd_temp[700];

const char http_header[600] = {
"POST / HTTP/1.1\r\n"
"Host: 192.165.1.78\r\n"
"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\n"
"Connection: keep-alive\r\n"
"Content-Type: application/x-www-form-urlencoded\r\n"
"Authorization: Basic 281A5F0B3C981119\r\n"
"Cookie: JSESSIONID=0123456789ABCDEF0123456789ABCDEF;\r\n"
"Content-MD5: E038\r\n"
"Content-Length: "
};

const char cmd_readparams[50]={"deviceJson={\"DevOpt\":{\"ReadParam\"}}"};
const char cmd_setparams[100]={"deviceJson={\"DevOpt\":{\"TransMode\":\"1\",\"HeartTime\":\"10\",\"Transport\":\"4\"}}"};
const char cmd_startatt[60]={"deviceJson={\"DevOpt\":{\"StartAttenceReading\":\"1\"}}"};
const char cmd_stopatt[60]={"deviceJson={\"DevOpt\":{\"StopAttenceReading\":\"1\"}}"};
const char cmd_clearbuff[60]={"deviceJson={\"DevOpt\":{\"ClearBuffer\":\"1\"}}"};

char* jsonCmdReadParams(void)
{
return jsonCmdMakeReq(cmd_readparams);
}

char* jsonCmdSetParams(void)
{
return jsonCmdMakeReq(cmd_setparams);
}


char* jsonCmdStartAttd(void)
{
return jsonCmdMakeReq(cmd_startatt);
}


char* jsonCmdStopAttd(void)
{
return jsonCmdMakeReq(cmd_stopatt);
}


char* jsonCmdClearBuff(void)
{
return jsonCmdMakeReq(cmd_clearbuff);
}


char* jsonCmdMakeReq(char *reqcmdstr)
{
int str_index = 0, cmdlen = 0;
memset(cmd_temp, 0, sizeof(cmd_temp));
str_index = sprintf(cmd_temp, "%s", http_header);
UARTprintf("\n--------------------------Str_index %d\n", str_index);
cmdlen = strlen(reqcmdstr);
str_index += sprintf(&cmd_temp[str_index],"%d",cmdlen);
str_index += sprintf(&cmd_temp[str_index],"\r\n\r\n%s",reqcmdstr);
return cmd_temp;
}

