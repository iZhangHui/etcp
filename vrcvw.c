#include "etcp.h"

int main( int argc, char **argv )
{
	SOCKET s;
	int n;
	char buf[ 128 ];
	WSABUF wbuf[ 2 ];
	DWORD sent;

	INIT();
	s = tcp_client( argv[ 1 ], argv[ 2 ] );
	wbuf[ 0 ].buf = ( char * )&n;
	wbuf[ 0 ].len = sizeof( n );
	wbuf[ 1 ].buf = buf;
	while ( fgets( buf, sizeof( buf ), stdin ) != NULL )
	{
		wbuf[ 1 ].len = strlen( buf );
		n = htonl( wbuf[ 1 ].len );
		if ( WSASend( s, wbuf, 2, &sent, 0, NULL, NULL ) < 0 )
			error( 1, errno, "WSASend failure" );
	}
	EXIT( 0 );
}
