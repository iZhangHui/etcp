#include "etcp.h"
int main( int argc, char **argv )
{
	struct sockaddr_in peer;
	int s;
	int rc;
	int len;
	int pidsz;
	char buf[ 120 ];

	pidsz = sprintf( buf, "%d: ", getpid() );
	len = sizeof( peer );
	rc = recvfrom( 0, buf + pidsz, sizeof( buf ) - pidsz,
		0, ( struct sockaddr * )&peer, &len );
	if ( rc < 0 )
		exit( 1 );

	s = socket( AF_INET, SOCK_DGRAM, 0 );
	if ( s < 0 )
		exit( 1 );
	if ( connect( s, ( struct sockaddr * )&peer, len ) < 0 )
		exit( 1 );
	if ( fork() != 0 )				/* error or parent? */
		exit( 0 );

	/* child process */

	while ( strncmp( buf + pidsz, "done", 4 ) != 0 )
	{
		if ( write( s, buf, rc + pidsz ) < 0 )
			break;
		pidsz = sprintf( buf, "%d: ", getpid() );
		alarm( 30 );
		rc = read( s, buf + pidsz, sizeof( buf ) - pidsz );
		alarm( 0 );
		if ( rc < 0 )
			break;
	}
	exit( 0 );
}
