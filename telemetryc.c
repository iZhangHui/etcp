#include "etcp.h"
int main( int argc, char **argv )
{
	SOCKET s;
	int rc;
	int i;
	int pkt[ 3 ];

	INIT();
	s = tcp_client( argv[ 1 ], argv[ 2 ] );
	for ( i = 2;; i = 5 - i )
	{
		pkt[ 0 ] = htonl( i );
		rc = send( s, ( char * )pkt, i * sizeof( int ), 0 );
		if ( rc < 0 )
			error( 1, errno, "send failure" );
		sleep( 1 );
	}
}
