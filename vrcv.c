#include "etcp.h"
#include <sys/uio.h>

int main( int argc, char **argv )
{
	SOCKET s;
	int n;
	char buf[ 128 ];
	struct iovec iov[ 2 ];

	INIT();
	s = tcp_client( argv[ 1 ], argv[ 2 ] );
	iov[ 0 ].iov_base = ( char * )&n;
	iov[ 0 ].iov_len = sizeof( n );
	iov[ 1 ].iov_base = buf;
	while ( fgets( buf, sizeof( buf ), stdin ) != NULL )
	{
		iov[ 1 ].iov_len = strlen( buf );
		n = htonl( iov[ 1 ].iov_len );
		if ( writev( s, iov, 2 ) < 0 )
			error( 1, errno, "writev failure" );
	}
	EXIT( 0 );
}
