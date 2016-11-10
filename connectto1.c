#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/time.h>
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


/* client - place holder for the client code */
static void client( SOCKET s, struct sockaddr_in *peerp )
{
	puts( "connected!" );
	CLOSE( s );
}

#ifdef WINDOWS
/* isconnectedw - windows version of isconnected */
/*@.sp .5 */
int isconnected( SOCKET s, fd_set *rd, fd_set *wr, fd_set *ex )
{
	WSASetLastError( 0 );
	if ( !FD_ISSET( s, rd ) && !FD_ISSET( s, wr ) )
		return 0;
	if ( FD_ISSET( s, ex ) )
		return 0;
	return 1;
}
#else
/* isconnected - check connect status */
/*@.sp .5 */
int isconnected( SOCKET s, fd_set *rd, fd_set *wr, fd_set *ex )
{
	int err;
	int len = sizeof( err );;

	errno = 0;			/* assume no error */
	if ( !FD_ISSET( s, rd ) && !FD_ISSET( s, wr ) )
		return 0;
	if ( getsockopt( s, SOL_SOCKET, SO_ERROR, &err, &len ) < 0 )
		return 0;
	errno = err;		/* in case we're not connected */
	return err == 0;
}
#endif

/* main - connect to the server */
int main( int argc, char **argv )
{
	fd_set rdevents;
	fd_set wrevents;
	fd_set exevents;
	struct sockaddr_in peer;
	struct timeval tv;
	SOCKET s;
	int flags;
	int rc;

	INIT();

	set_address( argv[ 1 ], argv[ 2 ], &peer, "tcp" );

	s = socket( AF_INET, SOCK_STREAM, 0 );
	if ( !isvalidsock( s ) )
		error( 1, errno, "socket call failed" );

	if( ( flags = fcntl( s, F_GETFL, 0 ) ) < 0 )
		error( 1, errno, "fcntl (F_GETFL) failed" );
	if ( fcntl( s, F_SETFL, flags | O_NONBLOCK ) < 0 )
		error( 1, errno, "fcntl (F_SETFL) failed" );

	if ( ( rc = connect( s, ( struct sockaddr * )&peer,
		 sizeof( peer ) ) ) && errno != EINPROGRESS )
		error( 1, errno, "connect failed" );
/*@.bp*/
	if ( rc == 0 )			/* already connected? */
	{
		if ( fcntl( s, F_SETFL, flags ) < 0 )
			error( 1, errno, "fcntl (restore flags) failed" );
		client( s, &peer );
		EXIT( 0 );
	}

	FD_ZERO( &rdevents );
	FD_SET( s, &rdevents );
	wrevents = rdevents;
	exevents = rdevents;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	rc = select( s + 1, &rdevents, &wrevents, &exevents, &tv );
	if ( rc < 0 )
		error( 1, errno, "select failed" );
	else if ( rc == 0 )
		error( 1, 0, "connect timed out\n" );
	else if ( isconnected( s, &rdevents, &wrevents, &exevents ) )
	{
		if ( fcntl( s, F_SETFL, flags ) < 0 )
			error( 1, errno, "fcntl (restore flags) failed" );
		client( s, &peer );
	}
	else
		error( 1, errno, "connect failed" );
	EXIT( 0 );
}
