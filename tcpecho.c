#include "etcp.h"

int main( int argc, char **argv )
{
	SOCKET s;
	SOCKET s1;
	char buf[ 1024 ];
	int rc;
	int nap = 0;

	INIT();
	if ( argc == 3 )
		nap = atoi( argv[ 2 ] );
	s = tcp_server( NULL, argv[ 1 ] );
	s1 = accept( s, NULL, NULL );
	if ( !isvalidsock( s1 ) )
		error( 1, errno, "accept failed" );
	signal( SIGPIPE, SIG_IGN );	/* report sigpipe to us */
	for ( ;; )
	{
		rc = recv( s1, buf, sizeof( buf ), 0 );
		if ( rc == 0 )
			error( 1, 0, "client disconnected\n" );
		if ( rc < 0 )
			error( 1, errno, "recv failed" );
		if ( nap )
			sleep( nap );
		rc = send( s1, buf, rc, 0 );
		if ( rc < 0 )
			error( 1, errno, "send failed" );
	}
}
