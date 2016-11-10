#include "etcp.h"
#define TWOINTS		( sizeof( int ) * 2 )
#define THREEINTS	( sizeof( int ) * 3 )
int main( int argc, char **argv )
{
	SOCKET s;
	SOCKET s1;
	int rc;
	int i = 1;
	int pkt[ 3 ];

	INIT();
	s = tcp_server( NULL, argv[ 1 ] );
	s1 = accept( s, NULL, NULL );
	if ( !isvalidsock( s1 ) )
		error( 1, errno, "accept failure" );
	for ( ;; )
	{
		rc = recv( s1, ( char * )pkt, sizeof( pkt ), 0 );
		if ( rc != TWOINTS && rc != THREEINTS )
			error( 1, 0, "recv returned %d\n", rc );
		printf( "Packet %d has %d values in %d bytes\n",
			i++, ntohl( pkt[ 0 ] ), rc );
	}
}
