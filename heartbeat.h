#ifndef __HEARTBEAT_H__
#define __HEARTBEAT_H__

#define MSG_TYPE1		1		/* application specific msg */
#define MSG_TYPE2		2		/* another one */
#define MSG_HEARTBEAT	3		/* heartbeat message */

typedef struct					/* message structure */
{
	u_int32_t type;				/* MSG_TYPE1, ... */
	char data[ 2000 ];
} msg_t;

#define T1				60		/* idle time before heartbeat */
#define T2				10		/* time to wait for response */

#endif  /* __HEARTBEAT_H__ */
