#include "etcp.h"
int main( int argc, char **argv )
{
	struct sockaddr_in peer;
	int rc;
	int len;
	int pidsz;
	char buf[ 120 ];

	pidsz = sprintf( buf, "%d: ", getpid() );
	len = sizeof( peer );
	rc = recvfrom( 0, buf + pidsz, sizeof( buf ) - pidsz, 0,
		( struct sockaddr * )&peer, &len );
	if ( rc <= 0 )
		exit( 1 );
	sendto( 1, buf, rc + pidsz, 0,
		( struct sockaddr * )&peer, len );
	exit( 0 );
}
