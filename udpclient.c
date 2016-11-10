#include "etcp.h"
int main( int argc, char **argv )
{
	struct sockaddr_in peer;
	SOCKET s;
	int rc = 0;
	int len;
	char buf[ 120 ];

	INIT();
	s = udp_client( argv[ 1 ], argv[ 2 ], &peer );
	while ( fgets( buf, sizeof( buf ), stdin ) != NULL )
	{
		rc = sendto( s, buf, strlen( buf ), 0,
			( struct sockaddr * )&peer, sizeof( peer ) );
		if ( rc < 0 )
			error( 1, errno, "sendto failed" );
		len = sizeof( peer );
		rc = recvfrom( s, buf, sizeof( buf ) - 1, 0,
			( struct sockaddr * )&peer, &len );
		if ( rc < 0 )
			error( 1, errno, "recvfrom failed" );
		buf[ rc ] = '\0';
		fputs( buf, stdout );
	}
	EXIT( 0 );
}
