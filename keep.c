#include "etcp.h"
int main( int argc, char **argv )
{
	SOCKET s;
	SOCKET s1;
	int on = 1;
	int rc;
	char buf[ 128 ];

	INIT();
	s = tcp_server( NULL, argv[ 1 ] );
	s1 = accept( s, NULL, NULL );
	if ( !isvalidsock( s1 ) )
		error( 1, errno, "accept failure\n" );
	if ( setsockopt( s1, SOL_SOCKET, SO_KEEPALIVE,
		( char * )&on, sizeof( on ) ) )
		error( 1, errno, "setsockopt failure" );
	for ( ;; )
	{
		rc = readline( s1, buf, sizeof( buf ) );
		if ( rc == 0 )
			error( 1, 0, "peer disconnected\n" );
		if ( rc < 0 )
			error( 1, errno, "recv failure" );
		write( 1, buf, rc );
	}
}
