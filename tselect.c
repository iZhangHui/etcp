/* include declarations */
#include "etcp.h"

#define NTIMERS 25

typedef struct tevent_t tevent_t;
struct tevent_t
{
	tevent_t *next;
	struct timeval tv;
	void ( *func )( void * );
	void *arg;
	unsigned int id;
};

static tevent_t *active = NULL;		/* active timers */
static tevent_t *free_list = NULL;	/* inactive timers */
/* end declarations */

/* include timeout */
static tevent_t *allocate_timer( void )
{
	tevent_t *tp;

	if ( free_list == NULL )	/* need new block of timers? */
	{
		free_list = malloc( NTIMERS * sizeof( tevent_t ) );
		if ( free_list == NULL )
			error( 1, 0, "couldn't allocate timers\n" );
		for ( tp = free_list;
			  tp < free_list + NTIMERS - 1; tp++ )
			tp->next = tp + 1;
		tp->next = NULL;
	}
	tp = free_list;				/* allocate first free */
	free_list = tp->next;		/* and pop it off list */
	return tp;
}

unsigned int timeout( void ( *func )( void * ), void *arg, int ms )
{
	tevent_t *tp;
	tevent_t *tcur;
	tevent_t **tprev;
	static unsigned int id = 1;			/* timer ID */

	tp = allocate_timer();
	tp->func = func;
	tp->arg = arg;
	if ( gettimeofday( &tp->tv, NULL ) < 0 )
		error( 1, errno, "timeout: gettimeofday failure" );
	tp->tv.tv_usec += ms * 1000;
	if ( tp->tv.tv_usec > 1000000 )
	{
		tp->tv.tv_sec += tp->tv.tv_usec / 1000000;
		tp->tv.tv_usec %= 1000000;
	}
	for ( tprev = &active, tcur = active;
		  tcur && !timercmp( &tp->tv, &tcur->tv, < ); /* XXX */
		  tprev = &tcur->next, tcur = tcur->next )
	{ ; }
	*tprev = tp;
	tp->next = tcur;
	tp->id = id++;				/* set ID for this timer */
	return tp->id;
}
/* end timeout */

/* untimeout - cancel a timer */
void untimeout( unsigned int id )
{
	tevent_t **tprev;
	tevent_t *tcur;

	for ( tprev = &active, tcur = active;
		  tcur && id != tcur->id;
		  tprev = &tcur->next, tcur = tcur->next )
	{ ; }
	if ( tcur == NULL )
	{
		error( 0, 0,
			"untimeout called for non-existent timer (%d)\n", id );
		return;
	}
	*tprev = tcur->next;
	tcur->next = free_list;
	free_list = tcur;
}

/* tselect - select with timers */
int tselect( int maxp1, fd_set *re, fd_set *we, fd_set *ee )
{
	fd_set rmask;
	fd_set wmask;
	fd_set emask;
	struct timeval now;
	struct timeval tv;
	struct timeval *tvp;
	tevent_t *tp;
	int n;

	if ( re )
		rmask = *re;
	if ( we )
		wmask = *we;
	if ( ee )
		emask = *ee;
	for ( ;; )
	{
		if ( gettimeofday( &now, NULL ) < 0 )
			error( 1, errno, "tselect: gettimeofday failure" );
		while ( active && !timercmp( &now, &active->tv, < ) )
		{
			active->func( active->arg );
			tp = active;
			active = active->next;
			tp->next = free_list;
			free_list = tp;
		}
		if ( active )
		{
			tv.tv_sec = active->tv.tv_sec - now.tv_sec;;
			tv.tv_usec = active->tv.tv_usec - now.tv_usec;
			if ( tv.tv_usec < 0 )
			{
				tv.tv_usec += 1000000;
				tv.tv_sec--;
			}
			tvp = &tv;
		}

		else if ( re == NULL && we == NULL && ee == NULL )
			return 0;
		else
			tvp = NULL;
		n = select( maxp1, re, we, ee, tvp );
		if ( n < 0 )
			return -1;
		if ( n > 0 )
			return n;
		if ( re )
			*re = rmask;
		if ( we )
			*we = wmask;
		if ( ee )
			*ee = emask;
	}
}

#ifdef TEST

struct timeval start;

void subtimers( struct timeval *t, struct timeval *u,
	struct timeval *v )
{
	v->tv_sec = t->tv_sec - u->tv_sec;
	v->tv_usec = t->tv_usec - u->tv_usec;
	if ( v->tv_usec < 0 )
	{
		v->tv_sec--;
		v->tv_usec += 1000000;
	}
}

void report( void *p )
{
	struct timeval elapsed;
	struct timeval now;
	int r = ( int )p;

	gettimeofday( &now, NULL );
	subtimers( &now, &start, &elapsed );
	printf( "call %d at %ld secs, %ld usecs\n",
		r, elapsed.tv_sec, elapsed.tv_usec );
}

int main( int argc, char **argv )
{
	fd_set rmask;
	unsigned int id;
	SOCKET s;
	char buf[ 128 ];

	INIT();
	s = udp_server( NULL, "9000" );

#ifdef WINDOWS
	FD_ZERO( &rmask );
	FD_SET( s, &rmask );
#endif

	gettimeofday( &start, NULL );
	timeout( report, ( void * )3, 3000 );
	timeout( report, ( void * )1, 500 );
	timeout( report, ( void * )2, 1500 );

#ifdef WINDOWS
	tselect( s + 1, &rmask, NULL, NULL );
	recvfrom( s, buf, sizeof( buf ), 0, NULL, NULL );
#else
	tselect( 0, NULL, NULL, NULL );
#endif
	printf( "socket is %s\n", FD_ISSET( s, &rmask ) ? "set" : "unset" );
	FD_ZERO( &rmask );
	FD_SET( s, &rmask );
	timeout( report, ( void * )5, 9000 );
	timeout( report, ( void * )4, 500 );
	tselect( s + 1, &rmask, NULL, NULL );
	recvfrom( s, buf, sizeof( buf ), 0, NULL, NULL );
	printf( "socket is %s\n", FD_ISSET( s, &rmask ) ? "set" : "unset" );
	id = timeout( report, ( void * )6, 1000 );

#ifdef WINDOWS
	FD_ZERO( &rmask );
	FD_SET( s, &rmask );
#endif

	timeout( report, ( void * )7, 500 );
	untimeout( id );

#ifdef WINDOWS
	tselect( s + 1, &rmask, NULL, NULL );
#else
	tselect( 0, NULL, NULL, NULL );
#endif
	printf( "socket is %s\n", FD_ISSET( s, &rmask ) ? "set" : "unset" );
	untimeout( 1000 );
	puts( "all done" );
	EXIT( 0 );
}

#endif
