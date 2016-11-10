/* include part1 */
#include "etcp.h"
#include "heartbeat.h"

int main( int argc, char **argv )
{
	fd_set allfd;
	fd_set readfd;
	char msg[ 1024 ];
	struct sockaddr_in peer;
	struct timeval tv;
	SOCKET s;
	SOCKET sdata;
	SOCKET shb;
	int rc;
	int maxfd1;
	int missed_heartbeats = 0;
	int peerlen = sizeof( peer );
	char hbmsg[ 1 ];

	INIT();
	s = tcp_server( NULL, argv[ 1 ] );
	sdata = accept( s, ( struct sockaddr * )&peer,
		&peerlen );
	if ( !isvalidsock( sdata ) )
		error( 1, errno, "accept failed" );
	rc = readn( sdata, ( char * )&peer.sin_port,
		sizeof( peer.sin_port ) );
	if ( rc < 0 )
		error( 1, errno, "error reading port number" );
	shb = socket( PF_INET, SOCK_STREAM, 0 );
	if ( !isvalidsock( shb ) )
		error( 1, errno, "shb socket failure" );
	rc = connect( shb, ( struct sockaddr * )&peer, peerlen );
	if ( rc )
		error( 1, errno, "shb connect error" );
	tv.tv_sec = T1 + T2;
	tv.tv_usec = 0;
	FD_ZERO( &allfd );
	FD_SET( sdata, &allfd );
	FD_SET( shb, &allfd );
	maxfd1 = ( sdata > shb ? sdata : shb ) + 1;
/* end part1 */
/* include part2 */
	for ( ;; )
	{
		readfd = allfd;
		rc = select( maxfd1, &readfd, NULL, NULL, &tv );
		if ( rc < 0 )
			error( 1, errno, "select failure" );
		if ( rc == 0 )		/* timed out */
		{
			if ( ++missed_heartbeats > 3 )
				error( 1, 0, "connection dead\n" );
			error( 0, 0, "missed heartbeat #%d\n",
				missed_heartbeats );
			tv.tv_sec = T2;
			continue;
		}
		if ( FD_ISSET( shb, &readfd ) )
		{
			rc = recv( shb, hbmsg, 1, 0 );
			if ( rc == 0 )
				error( 1, 0, "client terminated\n" );
			if ( rc < 0 )
				error( 1, errno, "shb recv failure" );
			rc = send( shb, hbmsg, 1, 0 );
			if ( rc < 0 )
				error( 1, errno, "shb send failure" );
		}
		if ( FD_ISSET( sdata, &readfd ) )
		{
			rc = recv( sdata, msg, sizeof( msg ), 0 );
			if ( rc == 0 )
				error( 1, 0, "client terminated\n" );
			if ( rc < 0 )
				error( 1, errno, "recv failure" );

			/* process data */
		}

		missed_heartbeats = 0;
		tv.tv_sec = T1 + T2;
	}
	EXIT( 0 );
}
/* end part2 */
