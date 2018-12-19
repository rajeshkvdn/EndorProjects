/* This file includes commands and response validation of  gsm gprs gps modules of SIM808 module */
#include <string.h>
#include <stdio.h>
#include "gprsgps.h"
#include "serial.h"
#include "config.h"
#include "utils/uartstdio.h"


#if(DEBUG_UART)
#define printf   UARTprintf
#endif


const char *at = {"AT\r\n"};
const char *ate0 = {"ATE0\r\n"};
const char *ate1 = {"ATE1\r\n"};
const char *at_cfun = {"AT+CFUN=1\r\n"};

/***GPS****/
const char *at_cgns_pwr = {"AT+CGNSPWR=1\r\n"};
const char *at_nmea_seq = {"AT+CGNSSEQ=\"RMC\"\r\n"};
const char *at_cgns_info = {"AT+CGNSINF\r\n"};

/*******GPRS**********/
const char *at_gprs_stat = {"AT+CGATT=1\r\n"};
const char *at_apn = {"AT+CSTT=\"bsnlnet\",\"\",\"\"\r\n"};
const char *at_conn = {"AT+CIICR\r\n"};
const char *at_ip_addr = {"AT+CIFSR\r\n"};
const char *at_start_conn = {"AT+CIPSTART=\"TCP\",\"122.165.154.209\",\"350\"\r\n"};
const char *at_send = {"AT+CIPSEND=\r\n"};
const char *at_ip_close = {"AT+CIPCLOSE\r\n"};
const char *at_ip_shut = {"AT+CIPSHUT\r\n"};

/********SMS**********************/
const char *at_sms_format = {"AT+CMGF=1\r\n"};
const char *at_sms_csmp = {"AT+CSMP=17,167,0,0\r\n"};
const char *at_cusd = {"AT+CUSD=0\r\n"};
const char *at_sms_send = {"AT+CMGS=\"+919446033603\"\r\n"};
const char *at_call_start = {"ATD9446033603;\r\n"};
const char *at_call_hold = {"ATH\r\n"};

/*
Brief   :   Sending request string to the uart device
Params  :   req - Pointer to the command string in specific format (here sim808)

Returns :  NIL
*/

void modemcmdReq(char *req)
{
int leng, i=0, count = 0;
char rbuff[512];
char cbuff[512];


memset(rbuff, 0, sizeof(rbuff));
memset(cbuff, 0, sizeof(cbuff));
strcpy(cbuff, req);
leng = strlen(cbuff);

i = 0;
while(i < leng)
    {
    SerialSend(0, cbuff[i]);
    i++;
    }
}

/*
Brief   :   Getting response from modem
Params  :   resp - response string address

Returns :  Length of response string
*/
int modemCmdResp(char *resp)
{
    int i=0, count = 0;
    static char tempbuff[256];

    count = (int)SerialReceiveAvailable(0);

    i = 0;
    while(count && (i < sizeof(tempbuff)))
        {
        tempbuff[i] = (char)SerialReceive(0);
        i++;
        count--;
        }
        strcpy(resp, tempbuff);
    return i;

}
/*
Brief   :       Basic AT command

Command :       AT
Response:       OK
Params  :       NIL
Return  :       1       - Success
                -1      - Read/Write fail or command ERROR
*/
void cmd_test(void)
{
    modemcmdReq(at);
}

/*
Brief   :       Echo Enable 
Return  :      NIL
*/
void cmd_echo_enable(void)
{
    modemcmdReq(ate1);
}


/*
Brief   :      Echo Disable
Return  :      NIL
*/
void cmd_echo_disable(void)
{
    modemcmdReq(ate1);
}


/*
Brief   :       Set Functionality of the modem

Command :       AT+CFUN=1
Response:       OK
Params  :       NIL
Return  :       NIL
*/
void cmd_func_set(void)
{
    modemcmdReq(at_cfun);
}

/*********************GPS COMMANDS***********************************/

/*
Brief   :       Turns on GNSS Power

Command :       AT+CGNSPWR=1
Response:       OK
Params  :       NIL
Return  :       NIL
*/
void cmd_gnss_pwr(void)
{
    modemcmdReq(at_cgns_pwr);
}


/*
Brief   :       parses the NMEA sentence related to GPRMC

Command :       AT+CGNSSEQ="RMC"
Response:       OK
Params  :       NIL
Return  :       NIL
*/
void cmd_nmea_seq(void)
{
    modemcmdReq(at_nmea_seq);
}


/*
Brief   :       Reading GPS information like lat,long,time, speed etc and passes it to a buffer for further processing

Command :       AT+CGNSINF
Response:       +CGNSINF: 1,1,20180926122848.000,9.970505,76.310982,243.500,1.52,13.1,1,,85.5,100.0,82.6,,8,3,,,32,,

                OK
Params  :       NIL
Return  :       NIL
*/
void cmd_cgns_info(void)
{
    modemcmdReq(at_cgns_info);
}


/*********************GPRS COMMANDS***********************************/

/*
Brief   :       GPRS service status

Command :       AT+CGATT?
Response:       OK
Params  :       NIL
Return  :       NIL
*/
void cmd_gprs_stat(void)
{
    modemcmdReq(at_gprs_stat);
}


/*
Brief   :       Set APN

Command :       AT+CSTT="bsnlnet","",""
Response:       OK
Params  :       NIL
Return  :       NIL
*/
void cmd_apn_set(void)
{
    modemcmdReq(at_apn);
}


/*
Brief   :       Bringup Wireless connections

Command :       AT+CIICR
Response:       OK
Params  :       NIL
Return  :       NIL
*/
void cmd_wl_conn(void)
{
    modemcmdReq(at_conn);
}


/*
Brief   :       Get local IP address

Command :       AT+CIFSR
Response:       ip address
Params  :       NIL
Return  :       NIL
*/
void cmd_ip_get(void)
{
    modemcmdReq(at_ip_addr);
}


/*
Brief   :       Start Connection

Command :       AT+CIPSTART="TCP","122.165.154.209","350"
Response:       OK
Params  :       NIL
Return  :      NIL
*/
void cmd_conn_start(void)
{
    modemcmdReq(at_start_conn);
}


/*
Brief   :       Send GPRS Data

Command :       AT+CIPSEND=<length>
Response:       SEND OK
Params  :       gprs
Return  :       NIL
*/
void cmd_send_gprsdata(char *sbuf)
{
int l, dlen;
char *idx;
char tempbuff[256];

memset(tempbuff, 0, sizeof(tempbuff));

dlen = strlen(sbuf);

//sprintf(tempbuff,"AT+CIPSEND=%d\r\n", dlen);

sprintf(tempbuff,"AT+CIPSEND\r\n");

modemcmdReq(tempbuff);

vTaskDelay(1000);

memset(tempbuff, 0, sizeof(tempbuff));
memcpy(tempbuff, sbuf, dlen);
tempbuff[dlen]=0x1A;

modemcmdReq(tempbuff);

}


/*
Brief   :       Close Connectio

Command :       AT+CIPCLOSE
Response:       CLOSE OK
Params  :       NIL
Return  :      NIL
*/
void cmd_conn_close(void)
{
    modemcmdReq(at_ip_close);
}



/*
Brief   :       Deactivate GPRS PDP context

Command :       AT+CIPSHUT
Response:       SHUT OK
Params  :       NIL
Return  :       1       - Success
                -1      - Read/Write fail or command ERROR
*/
void cmd_conn_shut(void)
{
    modemcmdReq(at_ip_shut);
}


/********************SMS COMMANDS********************/


/*
Brief   :       Set SMS format as text mode

Command :       AT+CMGF=1
Response:       OK
Params  :       NIL
Return  :       NIL
*/
void cmd_smsformat_set(void)
{
    modemcmdReq(at_sms_format);
}


/*
Brief   :       Set SMS text mode parameters

Command :       AT+CSMP=17,167,0,0
Response:       OK
Params  :       NIL
Return  :       NIL
*/
void cmd_smsmode_set(void)
{
    modemcmdReq(at_sms_csmp);
}


/*
Brief   :       Desable Supplementary service data

Command :       AT+CUSD=0
Response:       OK
Params  :       NIL
Return  :       NIL
*/
void cmd_serv_disable(void)
{
    modemcmdReq(at_cusd);
}

/*
Brief   :       Send Message

Command :       AT+CMGS="+91xxxxxxxx"
Response:       OK
Params  :       param1 - pointer to the phone number to which SMS send
		param2 - pointer to the message 
		param3 - length of mesage 
Return  :       NIL
*/
void cmd_sms_send(char *phno, char *msg, int len)
{
int l;
char *idx;
char rep[80], tembuf[100];
memset(rep, 0, sizeof(rep));
memset(tembuf, 0, sizeof(tembuf));

strcpy(tembuf, at_sms_send);
memcpy(&tembuf[12], phno, 10);

modemcmdReq(tembuf);

memset(rep, 0, sizeof(rep));
memset(tembuf, 0, sizeof(tembuf));

strncpy(tembuf, msg, len);
tembuf[len] = 0x1A;		/*CtrlZ*/

vTaskDelay(10);
modemcmdReq(tembuf);
}


/*
Brief   :       Make a voice call

Command :       ATDxxxxxxxx;
Response:       OK
Params  :       param1 - pointer to the phone number to which SMS send
Return  :       NIL
*/
void cmd_call_start(char *phno)
{
char tembuf[20];

memset(tembuf, 0, sizeof(tembuf));

strcpy(tembuf, at_call_start);
memcpy(&tembuf[3], phno, 10);

modemcmdReq(tembuf);
}

/*
Brief   :       Hold a voice call

Command :       ATH
Response:       OK
Params  :       NIL
Return  :       NIL
*/
void cmd_call_hold(void)
{
    modemcmdReq(at_call_hold);
}
