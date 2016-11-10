#include <stdio.h>

void main( void )
{
	int cnt = 0;
	char line[ 1024 ];

	/*
	 * We have to explicitly set line buffering since the stdio
	 * routines will not see the socket as a terminal device.
	 */

	setvbuf( stdout, NULL, _IOLBF, 0 );
	while ( fgets( line, sizeof( line ), stdin ) != NULL )
		printf( "%3i: %s", ++cnt, line );
}
