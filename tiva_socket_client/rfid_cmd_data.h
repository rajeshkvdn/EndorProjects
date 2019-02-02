/*
 * rfid_cmd_data.h
 *
 *  Created on: 05-Jan-2019
 *      Author: Rajesh
 */

#ifndef RFID_CMD_DATA_H_
#define RFID_CMD_DATA_H_


char* jsonCmdReadParams(void);
char* jsonCmdSetParams(void);
char* jsonCmdStartAttd(void);
char* jsonCmdStopAttd(void);
char* jsonCmdMakeReq(char *reqcmdstr);



#endif /* RFID_CMD_DATA_H_ */
