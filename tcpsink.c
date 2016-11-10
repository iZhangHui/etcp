/* include includes */
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
#include "etcp.h"

char *program_name;

/* end includes */
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

/* readn - read exactly n bytes */
int readn( SOCKET fd, char *bp, size_t len)
{
	int cnt;
	int rc;

	cnt = len;
	while ( cnt > 0 )
	{
		rc = recv( fd, bp, cnt, 0 );
		if ( rc < 0 )				/* read error? */
		{
			if ( errno == EINTR )	/* interrupted? */
				continue;			/* restart the read */
			return -1;				/* return error */
		}
		if ( rc == 0 )				/* EOF? */
			return len - cnt;		/* return short count */
		bp += rc;
		cnt -= rc;
	}
	return len;
}

/* server - place holder for server */
static void server( SOCKET s, int rcvbufsz )
{
	char *buf;
	int rc;
	int bytes = 0;

	if ( ( buf = malloc( rcvbufsz ) ) == NULL )
		error( 1, 0, "malloc failed\n" );
	for ( ;; )
	{
		rc = recv( s, buf, rcvbufsz, 0 );
		if ( rc <= 0 )
			break;
		bytes += rc;
	}
	error( 0, 0, "%d bytes received\n", bytes );
}

/* main - TCP setup, listen, and accept */
int main( int argc, char **argv )
{
	struct sockaddr_in local;
	struct sockaddr_in peer;
	int peerlen;
	SOCKET s1;
	SOCKET s;
	int c;
	int rcvbufsz = 32 * 1024;
	const int on = 1;

	INIT();
	opterr = 0;
	while ( ( c = getopt( argc, argv, "b:" ) ) != EOF )
	{
		switch ( c )
		{

			case 'b' :
				rcvbufsz = atoi( optarg );
				break;

			case '?' :
				error( 1, 0, "illegal option: %c\n", c );
		}
	}

	set_address( NULL, "9000", &local, "tcp" );
	s = socket( AF_INET, SOCK_STREAM, 0 );
	if ( !isvalidsock( s ) )
		error( 1, errno, "socket call failed" );

	if ( setsockopt( s, SOL_SOCKET, SO_REUSEADDR,
		( char * )&on, sizeof( on ) ) )
		error( 1, errno, "setsockopt SO_REUSEADDR failed" );

	if ( setsockopt( s, SOL_SOCKET, SO_RCVBUF,
		( char * )&rcvbufsz, sizeof( rcvbufsz ) ) )
		error( 1, errno, "setsockopt SO_RCVBUF failed" );

	if ( bind( s, ( struct sockaddr * ) &local,
		 sizeof( local ) ) )
		error( 1, errno, "bind failed" );

	listen( s, 5 );
	do
	{
		peerlen = sizeof( peer );
		s1 = accept( s, ( struct sockaddr * )&peer, &peerlen );
		if ( !isvalidsock( s1 ) )
			error( 1, errno, "accept failed" );
		server( s1, rcvbufsz );
		CLOSE( s1 );
	} while ( 0 );
	EXIT( 0 );
}
