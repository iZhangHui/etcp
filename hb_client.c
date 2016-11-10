#include "etcp.h"
#include "heartbeat.h"

int main( int argc, char **argv )
{
	fd_set allfd;
	fd_set readfd;
	msg_t msg;
	struct timeval tv;
	SOCKET s;
	int rc;
	int heartbeats = 0;
	int cnt = sizeof( msg );

	INIT();
	s = tcp_client( argv[ 1 ], argv[ 2 ] );
	FD_ZERO( &allfd );
	FD_SET( s, &allfd );
	tv.tv_sec = T1;
	tv.tv_usec = 0;
	for ( ;; )
	{
		readfd = allfd;
		rc = select( s + 1, &readfd, NULL, NULL, &tv );
		if ( rc < 0 )
			error( 1, errno, "select failure" );
		if ( rc == 0 )		/* timed out */
		{
			if ( ++heartbeats > 3 )
				error( 1, 0, "connection dead\n" );
			error( 0, 0, "sending heartbeat #%d\n", heartbeats );
			msg.type = htonl( MSG_HEARTBEAT );
			rc = send( s, ( char * )&msg, sizeof( msg ), 0 );
			if ( rc < 0 )
				error( 1, errno, "send failure" );
			tv.tv_sec = T2;
			continue;
		}
		if ( !FD_ISSET( s, &readfd ) )
			error( 1, 0, "select returned invalid socket\n" );
		rc = recv( s, ( char * )&msg + sizeof( msg ) - cnt,
			cnt, 0 );
		if ( rc == 0 )
			error( 1, 0, "server terminated\n" );
		if ( rc < 0 )
			error( 1, errno, "recv failure" );
		heartbeats = 0;
		tv.tv_sec = T1;
		cnt -= rc;					/* in-line readn */
		if ( cnt > 0 )
			continue;
		cnt = sizeof( msg );

		/* process message */
	}
}
