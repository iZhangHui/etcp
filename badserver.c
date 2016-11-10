#include "etcp.h"

int main( int argc, char **argv )
{
	struct sockaddr_in local;
	SOCKET s;
	SOCKET s1;
	int rc;
	char buf[ 1024 ];

	INIT();
	s = socket( PF_INET, SOCK_STREAM, 0 );
	if ( !isvalidsock( s ) )
		error( 1, errno, "Could not allocate socket" );
	bzero( &local, sizeof( local ) );
	local.sin_family = AF_INET;
	local.sin_port = htons( 9000 );
	local.sin_addr.s_addr = htonl( INADDR_ANY );
	if ( bind( s, ( struct sockaddr * )&local,
		sizeof( local ) ) < 0 )
		error( 1, errno, "Could not bind socket" );
	if ( listen( s, NLISTEN ) < 0 )
		error( 1, errno, "listen failed" );
	s1 = accept( s, NULL, NULL );
	if ( !isvalidsock( s1 ) )
		error( 1, errno, "accept failed" );
	for ( ;; )
	{
		rc = recv( s1, buf, sizeof( buf ), 0 );
		if ( rc < 0 )
			error( 1, errno, "recv failed" );
/*@.bp*/
		if ( rc == 0 )
			error( 1, 0, "Client disconnected\n" );
		rc = send( s1, buf, rc, 0 );
		if ( rc < 0 )
			error( 1, errno, "send failed" );
	}
}
