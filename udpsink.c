#include "etcp.h"
int main( int argc, char **argv )
{
	SOCKET s;
	int rc;
	int datagrams = 0;
	int rcvbufsz = 5000 * 1440;
	char buf[ 1440 ];

	INIT();
	s = udp_server( NULL, "9000" );
	setsockopt( s, SOL_SOCKET, SO_RCVBUF,
		( char * )&rcvbufsz, sizeof( int ) );
	for( ;; )
	{
		rc = recv( s, buf, sizeof( buf ), 0 );
		if ( rc <= 0 )
			break;
		datagrams++;
	}
	error( 0, 0, "%d datagrams received\n", datagrams );
	EXIT( 0 );
}
