#include "etcp.h"
int main( int argc, char **argv )
{
	SOCKET s;
	int rc;
	int len;
	char buf[ 120 ];

	INIT();
	s = tcp_client( argv[ 1 ], argv[ 2 ] );
	while ( fgets( buf, sizeof( buf ), stdin ) != NULL )
	{
		len = strlen( buf );
		rc = send( s, buf, len, 0 );
		if ( rc < 0 )
			error( 1, errno, "send failed" );
		rc = readline( s, buf, sizeof( buf ) );
		if ( rc < 0 )
			error( 1, errno, "readline failed" );
		else if ( rc == 0 )
			error( 1, 0, "server terminated\n" );
		else
			fputs( buf, stdout );
	}
	EXIT( 0 );
}
