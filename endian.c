#include <stdio.h>
#include <sys/types.h>
#include "etcp.h"
int main( void )
{
	u_int32_t x = 0x12345678;	/* 305419896 */
	unsigned char *xp = ( char * )&x;

	printf( "%0x %0x %0x %0x\n",
		xp[ 0 ], xp[ 1 ], xp[ 2 ], xp[ 3 ] );
	exit( 0 );
}
