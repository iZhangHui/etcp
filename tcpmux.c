/* include header */
#include "etcp.h"

#define MAXARGS		10		/* maximum arguments to server */
#define MAXLINE		256		/* maximum line size in tcpmux.conf */
#define NSERVTAB	10		/* number of service_table entries */
#define CONFIG		"tcpmux.conf"

typedef struct
{
	int	flag;
	char *service;
	char *path;
	char *args[ MAXARGS + 1 ];
} servtab_t;

int ls;						/* socket to listen on */
servtab_t service_table[ NSERVTAB + 1 ];

/* end header */

/* parsetab - parse the service table */
static void parsetab( void )
{
	FILE *fp;
	servtab_t *stp = service_table;
	char *cp;
	int i;
	int lineno;
	char line[ MAXLINE ];

	fp = fopen( CONFIG, "r" );
	if ( fp == NULL )
		error( 1, errno, "unable to open %s", CONFIG );
	lineno = 0;
	while ( fgets( line, sizeof( line ), fp ) != NULL )
	{
		lineno++;
		if ( line[ strlen( line ) - 1 ] != '\n' )
			error( 1, 0, "line %d is too long\n", lineno );
		if ( stp >= service_table + NSERVTAB )
			error( 1, 0, "too many entries in tcpmux.conf\n" );
		cp = strchr( line, '#' );
		if ( cp != NULL )
			*cp = '\0';
		cp = strtok( line, " \t\n" );
		if ( cp == NULL )
			continue;
		if ( *cp == '+' )
		{
			stp->flag = TRUE;
			cp++;
			if ( *cp == '\0' || strchr( " \t\n", *cp ) != NULL )
				error( 1, 0, "line %d: white space after `+'\n",
					lineno );
		}
		stp->service = strdup( cp );
		if ( stp->service == NULL )
			error( 1, 0, "out of memory\n" );
		cp = strtok( NULL, " \t\n" );
		if ( cp == NULL )
			error( 1, 0, "line %d: missing path name (%s)\n",
				lineno, stp->service );
		stp->path = strdup( cp );
		if ( stp->path == NULL )
			error( 1, 0, "out of memory\n" );
		for ( i = 0; i < MAXARGS; i++ )
		{
			cp = strtok( NULL, " \t\n" );
			if ( cp == NULL )
				break;
			stp->args[ i ] = strdup( cp );
			if ( stp->args[ i ] == NULL )
				error( 1, 0, "out of memory\n" );
		}
		if ( i >= MAXARGS && strtok( NULL, " \t\n" ) != NULL )
			error( 1, 0, "line %d: too many arguments (%s)\n",
				lineno, stp->service );
		stp->args[ i ] = NULL;
		stp++;
	}
	stp->service = NULL;
	fclose ( fp );
}

/* print_serv_tab - print the service table */
void print_serv_tab( void )
{
	servtab_t *stp;
	int i;

	for ( stp = service_table; stp->service; stp++ )
	{
		i = 0;
		printf( "%s, %s, %s\n", stp->service, stp->path,
		stp->flag ? "+": "-" );
		while ( stp->args[ i ] )
			printf( "\t%s\n", stp->args[ i++ ] );
	}
}

/* start_server - find and exec the requested server */
static void start_server( int s )
{
	char line[ MAXLINE ];
	servtab_t *stp;
	int rc;
	static char err1[] = "-unable to read service name\r\n";
	static char err2[] = "-unknown service\r\n";
	static char err3[] = "-unable to start service\r\n";
	static char ok[] = "+OK\r\n";

	rc = fork();
	if ( rc < 0 )		/* fork error */
	{
		write( s, err3, sizeof( err3 ) - 1 );
		return;
	}
	if ( rc != 0 )		/* parent */
		return;

	/* child process */

	CLOSE( ls );		/* close listening socket */

	alarm( 10 );
	rc = readcrlf( s, line, sizeof( line ) );
	alarm( 0 );
	if ( rc <= 0 )
	{
		write( s, err1, sizeof( err1 ) - 1 );
		EXIT( 1 );
	}

	for ( stp = service_table; stp->service; stp++ )
		if ( strcasecmp( line, stp->service ) == 0 )
			break;
	if ( !stp->service )
	{
		write( s, err2, sizeof( err2 ) - 1 );
		EXIT( 1 );
	}

	if ( stp->flag )
		if ( write( s, ok, sizeof( ok ) - 1 ) < 0 )
			EXIT( 1 );
	dup2( s, 0 );
	dup2( s, 1 );
	dup2( s, 2 );
	CLOSE( s );
	execv( stp->path, stp->args );
	write( 1, err3, sizeof( err3 ) - 1 );
	EXIT( 1 );
}

/* reaper - reap children */
void reaper( int sig )
{
	int waitstatus;

	while ( waitpid( -1, &waitstatus, WNOHANG ) > 0 )
				{;}
}

/* main - tcpmux superserver */
int main( int argc, char **argv )
{
	struct sockaddr_in peer;
	int s;
	int peerlen;

	/* Initialize and start the tcpmux server */

	INIT();
	parsetab();
	switch ( argc )
	{
		case 1:		/* default everything */
			ls = tcp_server( NULL, "tcpmux" );
			break;

		case 2:		/* specify interface, default port */
			ls = tcp_server( argv[ 1 ], "tcpmux" );
			break;

		case 3:		/* specify everything */
			ls = tcp_server( argv[ 1 ], argv[ 2 ] );
			break;

		default:
			error( 1, 0, "usage: %s [ interface [ port ] ]\n",
				program_name );
	}
	daemon( 0, 0 );
	signal( SIGCHLD, reaper );

	/* Accept connections to tcpmux port */

	for ( ;; )
	{
		peerlen = sizeof( peer );
		s = accept( ls, ( struct sockaddr * )&peer, &peerlen );
		if ( s < 0 )
			continue;
		start_server( s );
		CLOSE( s );
	}
}
