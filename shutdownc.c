#include "etcp.h"
int main( int argc, char **argv )
{
	SOCKET s;
	fd_set readmask;
	fd_set allreads;
	int rc;
	int len;
	int c;
	int closeit = FALSE;
	int err = FALSE;
	char lin[ 1024 ];
	char lout[ 1024 ];

	INIT();
	opterr = FALSE;
	while ( ( c = getopt( argc, argv, "c" ) ) != EOF )
	{
		switch( c )
		{
			case 'c' :
				closeit = TRUE;
				break;

			case '?' :
				err = TRUE;
		}
	}
	if ( err || argc - optind != 2 )
		error( 1, 0, "usage: %s [-c] host port\n",
			program_name );
	s = tcp_client( argv[ optind ], argv[ optind + 1 ] );
	FD_ZERO( &allreads );
	FD_SET( 0, &allreads );
	FD_SET( s, &allreads );
	for ( ;; )
	{
		readmask = allreads;
		rc = select( s + 1, &readmask, NULL, NULL, NULL );
		if ( rc <= 0 )
			error( 1, errno, "bad select return (%d)", rc );
		if ( FD_ISSET( s, &readmask ) )
		{
			rc = recv( s, lin, sizeof( lin ) - 1, 0 );
			if ( rc < 0 )
				error( 1, errno, "recv error" );
			if ( rc == 0 )
				error( 1, 0, "server disconnected\n" );
			lin[ rc ] = '\0';
			if ( fputs( lin, stdout ) )
				error( 1, errno, "fputs failed" );
		}
		if ( FD_ISSET( 0, &readmask ) )
		{
			if ( fgets( lout, sizeof( lout ), stdin ) == NULL )
			{
				FD_CLR( 0, &allreads );
				if ( closeit )
				{
					CLOSE( s );
					sleep( 5 );
					EXIT( 0 );
				}
				else if ( shutdown( s, 1 ) )
					error( 1, errno, "shutdown failed" );
			}
			else
			{
				len = strlen( lout );
				rc = send( s, lout, len, 0 );
				if ( rc < 0 )
					error( 1, errno, "send error" );
			}
		}
	}
}
