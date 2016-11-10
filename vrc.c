#include "etcp.h"

int main( int argc, char **argv )
{
	SOCKET s;
	int n;
	struct
	{
		u_int32_t reclen;
		char buf[ 128 ];
	} packet;

	INIT();
	s = tcp_client( argv[ 1 ], argv[ 2 ] );
	while ( fgets( packet.buf, sizeof( packet.buf ), stdin )
		!= NULL )
	{
		n = strlen( packet.buf );
		packet.reclen = htonl( n );
		if ( send( s, ( char * )&packet,
			 n + sizeof( packet.reclen ), 0 ) < 0 )
			error( 1, errno, "send failure" );
	}
	EXIT( 0 );
}
