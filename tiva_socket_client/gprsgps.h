#ifndef __GPRSGPS_H__
#define __GPRSGPS_H__

#define MODEM_CMD_DELAY 1

void modemcmdPutReq(char *req);
int modemCmdGetResp(char *resp);
void cmd_test(void);
void cmd_echo_enable(void);
void cmd_echo_disable(void);
void cmd_func_set(void);
void cmd_gnss_pwr(void);
void cmd_nmea_seq(void);
void cmd_cgns_info(void);
void cmd_gprs_stat(void);
void cmd_apn_set(void);
void cmd_wl_conn(void);
void cmd_ip_get(void);
void cmd_conn_start(void);
void cmd_send_gprsdata(char *sbuf);
void cmd_conn_close(void);
void cmd_conn_shut(void);
void cmd_smsformat_set(void);
void cmd_smsmode_set(void);
void cmd_serv_disable(void);
void cmd_sms_send(char *phno, char *msg, int len);
void cmd_call_start(char *phno);
void cmd_call_hold(void);
#endif
