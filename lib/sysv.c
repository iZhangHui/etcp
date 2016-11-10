/*
 *  Compatability routines when compiling under SysVR4
 *
 */

#include "../etcp.h"

/* inet_aton - version of inet_aton for SVr4 */
int inet_aton( const char *cp, struct in_addr *pin )
{
    int rc;
	 
	rc = inet_addr( cp );
	if ( rc == -1 && strcmp( cp, "255.255.255.255" ) )
		return 0;
	pin->s_addr = rc;
	return 1;
}

/* daemon - version of the BSD daemon function for SVr4 */
int daemon( int nocd, int noclose )
{
	struct rlimit rlim;
	pid_t pid;
	int i;

	umask( 0 );		/* clear file creation mask */

	/* Get max files that can be open */

	if ( getrlimit( RLIMIT_NOFILE, &rlim ) < 0 )
		error( 1, errno, "getrlimit failed" );

	/* Become session leader, losing controlling terminal */

	pid = fork();
	if ( pid < 0 )
		return -1;
	if ( pid != 0 )
		exit( 0 );
	setsid();

	/* ... and make sure we don't aquire another */

	signal( SIGHUP, SIG_IGN );
	pid = fork();
	if ( pid < 0 )
		return -1;
	if ( pid != 0 )
		exit( 0 );

	/* Change to root directory unless asked not to */

	if ( !nocd )
		chdir( "/" );

	/*
	 * Unless asked not to, close all files.
	 * Then dup stdin, stdout, and stderr
	 * onto /dev/null.
	 */

	if ( !noclose )
	{
#if 0	/* change to 1 to close all files */
		if ( rlim.rlim_max == RLIM_INFINITY )
			rlim.rlim_max = 1024;
		for ( i = 0; i < rlim.rlim_max; i++ )
			close( i );
#endif
		i = open( "/dev/null", O_RDWR );
		if ( i < 0 )
			return -1;
		dup2( i, 0 );
		dup2( i, 1 );
		dup2( i, 2 );
		if ( i > 2 )
			close( i );
	}
	return 0;
}

/* signal - a reliable version for SVR4 and others */
typedef void sighndlr_t( int );
sighndlr_t *signal( int sig, sighndlr_t *hndlr )
{
	struct sigaction act;
	struct sigaction xact;

	act.sa_handler = hndlr;
	act.sa_flags = 0;
	sigemptyset( &act.sa_mask );
	if ( sigaction( sig, &act, &xact ) < 0 )
		return SIG_ERR;
	return xact.sa_handler;
}
