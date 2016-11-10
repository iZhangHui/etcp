#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "skel.h"

char *program_name;

/* error - print a diagnostic and optionally exit */
void error( int status, int err, char *fmt, ... )
{
	va_list ap;

	va_start( ap, fmt );
	fprintf( stderr, "%s: ", program_name );
	vfprintf( stderr, fmt, ap );
	va_end( ap );
	if ( err )
		fprintf( stderr, ": %s (%d)\n", strerror( err ), err );
	if ( status )
		EXIT( status );
}

/* set_address - fill in a sockaddr_in structure */
static void set_address( char *hname, char *sname,
	struct sockaddr_in *sap, char *protocol )
{
	struct servent *sp;
	struct hostent *hp;
	char *endptr;
	short port;

	bzero( sap, sizeof( *sap ) );
	sap->sin_family = AF_INET;
	if ( hname != NULL )
	{
		if ( !inet_aton( hname, &sap->sin_addr ) )
		{
			hp = gethostbyname( hname );
			if ( !hp )
				error( 1, 0, "unknown host: %s\n", hname );
			sap->sin_addr = *( struct in_addr * )hp->h_addr;
		}
	}
	else
		sap->sin_addr.s_addr = htonl( INADDR_ANY );
	port = strtol( sname, &endptr, 0 );
	if ( *endptr == '\0' )
		sap->sin_port = htons( port );
	else
	{
		sp = getservbyname( sname, protocol );
		if ( !sp )
			error( 1, 0, "unknown service: %s\n", sname );
		sap->sin_port = sp->s_port;
	}
}


/* main - connect to the server */
int main( int argc, char **argv )
{
	struct sockaddr_in peer;
	char *buf;
	SOCKET s;
	int c;
	int blks = 5000;
	int sndbufsz = 32 * 1024;
	int sndsz = 1440;	/* default ethernet mss */

	INIT();
	opterr = 0;
	while ( ( c = getopt( argc, argv, "s:b:c:" ) ) != EOF )
	{
		switch ( c )
		{
			case 's' :
				sndsz = atoi( optarg );
				break;

			case 'b' :
				sndbufsz = atoi( optarg );
				break;

			case 'c' :
				blks = atoi( optarg );
				break;

			case '?' :
				error( 1, 0, "illegal option: %c\n", c );
		}
	}
	if ( argc <= optind )
		error( 1, 0, "missing host name\n" );

	if ( ( buf = malloc( sndsz ) ) == NULL )
		error( 1, 0, "malloc failed\n" );
	set_address( argv[ optind ], "9000", &peer, "tcp" );
	s = socket( AF_INET, SOCK_STREAM, 0 );
	if ( !isvalidsock( s ) )
		error( 1, errno, "socket call failed" );

	if ( setsockopt( s, SOL_SOCKET, SO_SNDBUF,
		( char * )&sndbufsz, sizeof( sndbufsz ) ) )
		error( 1, errno, "setsockopt SO_SNDBUF failed" );

	if ( connect( s, ( struct sockaddr * )&peer,
		 sizeof( peer ) ) )
		error( 1, errno, "connect failed" );

	while( blks-- > 0 )
		send( s, buf, sndsz, 0 );
	EXIT( 0 );
}
