#include "../etcp.h"

/* readcrlf - read a CR/LF terminated line */
int readcrlf( SOCKET s, char *buf, size_t len )
{
	char *bufx = buf;
	int rc;
	char c;
	char lastc = 0;

	while ( len > 0 )
	{
		if ( ( rc = recv( s, &c, 1, 0 ) ) != 1 )
		{
			/*
			 *  If we were interrupted, keep going,
			 *  otherwise, return EOF or the error.
			 */

			if ( rc < 0 && errno == EINTR )
				continue;
			return rc;
		}
		if ( c == '\n' )
		{
			if ( lastc == '\r' )
				buf--;
			*buf = '\0';			/* don't include <CR><LF> */
			return buf - bufx;
		}

		*buf++ = c;
		lastc = c;
		len--;
	}
	set_errno( EMSGSIZE );
	return -1;
}
