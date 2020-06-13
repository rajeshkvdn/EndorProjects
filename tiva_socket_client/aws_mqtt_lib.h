#ifndef __AWS_MQTT_LIB_H__
#define __AWS_MQTT_LIB_H__


#define MQTT_PROTOCOL_LEVEL		3

#define MQTT_CTRL_CONNECT		0x1
#define MQTT_CTRL_CONNECTACK		0x2
#define MQTT_CTRL_PUBLISH		0x3
#define MQTT_CTRL_PUBACK		0x4
#define MQTT_CTRL_PUBREC		0x5
#define MQTT_CTRL_PUBREL		0x6
#define MQTT_CTRL_PUBCOMP		0x7
#define MQTT_CTRL_SUBSCRIBE		0x8
#define MQTT_CTRL_SUBACK		0x9
#define MQTT_CTRL_UNSUBSCRIBE		0xA
#define MQTT_CTRL_UNSUBACK		0xB
#define MQTT_CTRL_PINGREQ		0xC
#define MQTT_CTRL_PINGRESP		0xD
#define MQTT_CTRL_DISCONNECT		0xE

#define MQTT_QOS_1			0x1
#define MQTT_QOS_0			0x0

/* Adjust as necessary, in seconds */
#define MQTT_CONN_KEEPALIVE		60

#define MQTT_CONN_USERNAMEFLAG		0x80
#define MQTT_CONN_PASSWORDFLAG		0x40
#define MQTT_CONN_WILLRETAIN		0x20
#define MQTT_CONN_WILLQOS_1		0x08
#define MQTT_CONN_WILLQOS_2		0x18
#define MQTT_CONN_WILLFLAG		0x04
#define MQTT_CONN_CLEANSESSION		0x02

#define DEFAULT_BUFFER_SIZE		200
#define DEFAULT_TIMEOUT			10000
#define DEFAULT_CRLF_COUNT		2

#define MINTHR              8000
#define RESOLUTION          488

#define InternalOsc_8MHz    8000000
#define InternalOsc_4MHz    4000000
#define InternalOsc_2MHz    2000000
#define InternalOsc_1MHz    1000000
#define InternalOsc_500KHz  500000
#define InternalOsc_250KHz  250000
#define InternalOsc_125KHz  125000
#define InternalOsc_31KHz   31000

#define Timer2Prescale_1    1
#define Timer2Prescale_4    4
#define Timer2Prescale_16   16

/* Select Demo */
#define SUBSRCIBE_DEMO				/* Define SUBSRCIBE demo */
//#define PUBLISH_DEMO				/* Define PUBLISH demo */

#define AIO_SERVER		"io.adafruit.com"	/* Adafruit server */
#define AIO_SERVER_PORT		"1883"			/* Server port */
#define AIO_BASE_URL		"/api/v2"		/* Base URL for api */
#define AIO_USERNAME		"ENDOR"	/* Enter username here */
#define AIO_KEY			"endor_student_tracker"		/* Enter AIO key here */
#define AIO_FEED		"Enter Feed Key"	/* Enter feed key */

#define APN			"internet"
#define USERNAME		""
#define PASSWORD		""

typedef unsigned short uint16_t;
typedef unsigned char uint8_t;


uint16_t MQTT_connectpacket(uint8_t* packet);

uint16_t MQTT_publishPacket(uint8_t *packet, char *topic, char *data, uint8_t qos);

#endif /*__AWS_MQTT_LIB_H__*/
