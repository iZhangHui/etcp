#include "../etcp.h"

/* readn - read exactly n bytes */
int readn( SOCKET fd, char *bp, size_t len)
{
	int cnt;
	int rc;

	cnt = len;
	while ( cnt > 0 )
	{
		rc = recv( fd, bp, cnt, 0 );
/*@.bp*/
		if ( rc < 0 )				/* read error? */
		{
			if ( errno == EINTR )	/* interrupted? */
				continue;			/* restart the read */
			return -1;				/* return error */
		}
		if ( rc == 0 )				/* EOF? */
			return len - cnt;		/* return short count */
		bp += rc;
		cnt -= rc;
	}
	return len;
}
