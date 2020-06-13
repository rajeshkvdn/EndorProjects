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


const char *at = {"AT\r\n"}; //4
const char *ate0 = {"ATE0\r\n"}; //6
const char *ate1 = {"ATE1\r\n"}; //6
const char *at_cfun = {"AT+CFUN=1\r\n"};  //11

/***GPS****/
const char *at_cgns_pwr = {"AT+CGNSPWR=1\r\n"}; //14
const char *at_nmea_seq = {"AT+CGNSSEQ=\"RMC\"\r\n"}; //18
const char *at_cgns_info = {"AT+CGNSINF\r\n"}; //12

/*******GPRS**********/
const char *at_gprs_stat = {"AT+CGATT=1\r\n"}; //12
const char *at_apn = {"AT+CSTT=\"bsnlnet\",\"\",\"\"\r\n"};  //23
const char *at_conn = {"AT+CIICR\r\n"}; //10
const char *at_ip_addr = {"AT+CIFSR\r\n"};  //10
#if AWS_TEST
const char *at_start_conn = {"AT+CIPSTART=\"TCP\",\"34.208.235.17\",\"8883\"\r\n"}; //42
#else
const char *at_start_conn = {"AT+CIPSTART=\"TCP\",\"122.165.154.209\",\"350\"\r\n"};
#endif
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
const char *at_secure_conn = {"AT+CIPSSL=1\r\n"};

/*
Brief   :   Sending request string to the uart device
Params  :   req - Pointer to the command string in specific format (here sim808)

Returns :  NIL
*/

void modemcmdPutReq(uint8_t *req, uint8_t len)
{
int leng, i=0, count = 0;
//char rbuff[1200];
uint8_t cbuff[1200];


//memset(rbuff, 0, sizeof(rbuff));
memset(cbuff, 0, sizeof(cbuff));
memcpy(cbuff, req, len);
leng = len;

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
int modemCmdGetResp(char *resp)
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
    modemcmdPutReq(at, 4);
}

/*
Brief   :       Echo Enable 
Return  :      NIL
*/
void cmd_echo_enable(void)
{
    modemcmdPutReq(ate1, strlen(ate1));
}


/*
Brief   :      Echo Disable
Return  :      NIL
*/
void cmd_echo_disable(void)
{
    modemcmdPutReq(ate1, strlen(ate1));
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
    modemcmdPutReq(at_cfun, strlen(at_cfun));
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
    modemcmdPutReq(at_cgns_pwr, strlen(at_cgns_pwr));
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
    modemcmdPutReq(at_nmea_seq, strlen(at_nmea_seq));
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
    modemcmdPutReq(at_cgns_info, strlen(at_cgns_info));
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
    modemcmdPutReq(at_gprs_stat, strlen(at_gprs_stat));
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
    modemcmdPutReq(at_apn, strlen(at_apn));
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
    modemcmdPutReq(at_conn, strlen(at_conn));
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
    modemcmdPutReq(at_ip_addr, strlen(at_ip_addr));
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
    modemcmdPutReq(at_start_conn, strlen(at_start_conn));
}


/*
Brief   :       Send GPRS Data

Command :       AT+CIPSEND=<length>
Response:       SEND OK
Params  :       gprs
Return  :       NIL
*/
void cmd_send_gprsdata(uint8_t *sbuf, uint8_t siz)
{
int l, dlen;
char *idx;
uint8_t tempbuff[800];

memset(tempbuff, 0, sizeof(tempbuff));

//dlen = strlen(sbuf);

//sprintf(tempbuff,"AT+CIPSEND=%d\r\n", dlen);

sprintf(tempbuff,"AT+CIPSEND\r\n");

modemcmdPutReq(tempbuff, 12);

vTaskDelay(1000);

memset(tempbuff, 0, sizeof(tempbuff));
memcpy(tempbuff, sbuf, siz);
tempbuff[siz]=0x1A;

modemcmdPutReq(tempbuff, siz+1);

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
    modemcmdPutReq(at_ip_close, strlen(at_ip_close));
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
    modemcmdPutReq(at_ip_shut, strlen(at_ip_shut));
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
    modemcmdPutReq(at_sms_format, strlen(at_sms_format));
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
    modemcmdPutReq(at_sms_csmp, strlen(at_sms_csmp));
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
    modemcmdPutReq(at_cusd, strlen(at_cusd));
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

void cmd_sms_send(char *phno, char *msg)
{
int l;
char *idx;
char tbuf[200];
//memset(rep, 0, sizeof(rep));
memset(tbuf, 0, sizeof(tbuf));

strcpy(tbuf, at_sms_send);
memcpy(&tbuf[12], phno, 10);

modemcmdPutReq(tbuf, strlen(tbuf));
vTaskDelay(1000);
l = strlen(msg);
//memset(rep, 0, sizeof(rep));
memset(tbuf, 0, sizeof(tbuf));

memcpy(tbuf, msg, l);
tbuf[l] = 0x1A;		/*CtrlZ*/

modemcmdPutReq(tbuf, l+1);
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

modemcmdPutReq(tembuf, strlen(tembuf));
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
    modemcmdPutReq(at_call_hold, strlen(at_call_hold));
}

/*
Brief   :       SEt SSL SEcurity On

Command :       AT+CIPSSL=1
Response:       OK
Params  :       NIL
Return  :       NIL
*/
void cmd_ssl_set(void)
{
    modemcmdPutReq(at_secure_conn, strlen(at_secure_conn));
}
