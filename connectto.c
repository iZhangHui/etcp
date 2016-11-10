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
#include <signal.h>
#include "etcp.h"

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

/* readline - read a newline terminated record */
int readline( SOCKET fd, char *bufptr, size_t len )
{
	char *bufx = bufptr;
	static char *bp;
	static int cnt = 0;
	static char b[ 1500 ];
	char c;

	while ( --len > 0 )
	{
		if ( --cnt <= 0 )
		{
			cnt = recv( fd, b, sizeof( b ), 0 );
			if ( cnt < 0 )
			{
				if ( errno == EINTR )
					continue;
				return -1;
			}
			if ( cnt == 0 )
				return 0;
			bp = b;
		}
		c = *bp++;
		*bufptr++ = c;
		if ( c == '\n' )
		{
			*bufptr = '\0';
			return bufptr - bufx;
		}
	}
	errno = EMSGSIZE;
	return -1;
}
/* client - place holder for the client code */
static void client( SOCKET s, struct sockaddr_in *peerp )
{
	char buf[ 128 ];

	readline( s, buf, sizeof( buf ) );
	puts( buf );
}

/* alarm_hndlr - catch the alarm signal */
void alarm_hndlr( int sig )
{
	return;
}

/* main - connect to the server */
/* include main_head */
int main( int argc, char **argv )
{
/* end main_head */
	struct sockaddr_in peer;
	SOCKET s;
	int rc;

	INIT();

	set_address( argv[ 1 ], argv[ 2 ], &peer, "tcp" );

	s = socket( AF_INET, SOCK_STREAM, 0 );
	if ( !isvalidsock( s ) )
		error( 1, errno, "socket call failed" );

/* include connect */
	signal( SIGALRM, alarm_hndlr );
	alarm( 5 );
	rc = connect( s, ( struct sockaddr * )&peer, sizeof( peer ) );
	alarm( 0 );
/*@.bp*/
	if ( rc < 0 )
	{
		if ( errno == EINTR )
			error( 1, 0, "connect timed out\n" );
/* end connect */
		error( 1, errno, "connect failed" );
	}
	client( s, &peer );
	EXIT( 0 );
/* include main_tail */
}
/* end main_tail */
