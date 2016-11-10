#include "etcp.h"

#define ACK		0x6		/* an ACK character */

int main( int argc, char **argv )
{
	fd_set allreads;
	fd_set readmask;
	fd_set sockonly;
	struct timeval tv;
	struct timeval *tvp = NULL;
	SOCKET s;
	int rc;
	char buf[ 128 ];
	const static struct timeval T0 = { 2, 0 };

	INIT();
	s = tcp_client( argv[ 1 ], argv[ 2 ] );
	FD_ZERO( &allreads );
	FD_SET( s, &allreads );
	sockonly = allreads;
	FD_SET( 0, &allreads );
	readmask = allreads;
	for ( ;; )
	{
		rc = select( s + 1, &readmask, NULL, NULL, tvp );
		if ( rc < 0 )
			error( 1, errno, "select failure" );
		if ( rc == 0 )
			error( 1, 0, "message timed out\n" );

		if ( FD_ISSET( s, &readmask ) )
		{
			rc = recv( s, buf, sizeof( buf ), 0 );
			if ( rc == 0 )
				error( 1, 0, "server disconnected\n" );
			else if ( rc < 0 )
				error( 1, errno, "recv failure" );
			else if ( rc != 1 || buf[ 0 ] != ACK )
				error( 1, 0, "unexpected input [%c]\n", buf[ 0 ] );
			tvp = NULL;				/* turn timer off */
			readmask = allreads;	/* and stdin on */
		}

		if ( FD_ISSET( 0, &readmask ) )
		{
			rc = read( 0, buf, sizeof( buf ) );
			if ( rc < 0 )
				error( 1, errno, "read failure" );
			if ( send( s, buf, rc, 0 ) < 0 )
				error( 1, errno, "send failure" );
			tv = T0;				/* reset timer */
			tvp = &tv;				/* turn timer on */
			readmask = sockonly;	/* and stdin off */
		}
	}
}

