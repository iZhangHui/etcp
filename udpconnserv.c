#include "etcp.h"
int main( int argc, char **argv )
{
	struct sockaddr_in peer;
	SOCKET s;
	int rc;
	int len;
	char buf[ 120 ];

	INIT();
	s = udp_server( NULL, argv[ 1 ] );
	len = sizeof( peer );
	rc = recvfrom( s, buf, sizeof( buf ),
		0, ( struct sockaddr * )&peer, &len );
	if ( rc < 0 )
		error( 1, errno, "recvfrom failed" );
/*@.bp*/
	if ( connect( s, ( struct sockaddr * )&peer, len ) )
		error( 1, errno, "connect failed" );

	while ( strncmp( buf, "done", 4 ) != 0 )
	{
		if ( send( s, buf, rc, 0 ) < 0 )
			error( 1, errno, "send failed" );
		rc = recv( s, buf, sizeof( buf ), 0 );
		if ( rc < 0 )
			error( 1, errno, "recv failed" );
	}
	EXIT( 0 );
}
