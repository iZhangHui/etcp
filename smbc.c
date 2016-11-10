#include "etcp.h"

int main( int argc, char **argv )
{
	char *bp;
	SOCKET s;

	INIT();
	s = tcp_client( argv[ 1 ], argv[ 2 ] );
	init_smb( FALSE );
	bp = smballoc();
	while ( fgets( bp, SMBUFSZ, stdin ) != NULL  )
	{
		smbsend( s, bp );
		bp = smballoc();
	}
	EXIT( 0 );
}
