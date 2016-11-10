#include "etcp.h"

int main( int argc, char **argv )
{
	fd_set allreads;
	fd_set readmask;
	SOCKET s;
	int rc;
	char buf[ 128 ];

	INIT();
	s = tcp_client( argv[ 1 ], argv[ 2 ] );
	FD_ZERO( &allreads );
	FD_SET( s, &allreads );
	FD_SET( 0, &allreads );
	for ( ;; )
	{
		readmask = allreads;
		rc = select( s + 1, &readmask, NULL, NULL, NULL );
		if ( rc <= 0 )
			error( 1, rc ? errno : 0, "select returned %d", rc );

		if ( FD_ISSET( 0, &readmask ) )
		{
			rc = read( 0, buf, sizeof( buf ) - 1 );
			if ( rc < 0 )
				error( 1, errno, "read failure" );
			if ( send( s, buf, rc, 0 ) < 0 )
				error( 1, errno, "send failure" );
		}

		if ( FD_ISSET( s, &readmask ) )
		{
			rc = recv( s, buf, sizeof( buf ) - 1, 0 );
			if ( rc == 0 )
				error( 1, 0, "server disconnected\n" );
			else if ( rc < 0 )
				error( 1, errno, "recv failure" );
			else
			{
				buf[ rc ] = '\0';
				error( 1, 0, "unexpected input [%s]\n", buf );
			}
		}
	}
}

