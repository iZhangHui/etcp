#include "etcp.h"

int main( int argc, char **argv )
{
	char *bp;
	SOCKET s;
	SOCKET s1;

	INIT();
	init_smb( TRUE );
	s = tcp_server( NULL, argv[ 1 ] );
	s1 = accept( s, NULL, NULL );
	if ( !isvalidsock( s1 ) )
		error( 1, errno, "accept failure" );
	for ( ;; )
	{
		bp = smbrecv( s1 );
		fputs( bp, stdout );
		smbfree( bp );
	}
	EXIT( 0 );
}
