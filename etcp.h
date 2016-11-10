#ifndef __ETCP_H__
#define __ETCP_H__

/* Include standard headers */

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "skel.h"

#define TRUE			1
#define FALSE			0
#define NLISTEN			5		/* max waiting connections */
#define NSMB			5		/* number shared memory bufs */
#define SMBUFSZ			256		/* size of shared memory buf */

extern char *program_name;		/* for error */

#ifdef __SVR4
#define bzero(b,n)	memset( ( b ), 0, ( n ) )
#endif 

typedef void ( *tofunc_t )( void * );

void error( int, int, char*, ... );
int readn( SOCKET, char *, size_t );
int readvrec( SOCKET, char *, size_t );
int readcrlf( SOCKET, char *, size_t );
int readline( SOCKET, char *, size_t );
int tcp_server( char *, char * );
int tcp_client( char *, char * );
int udp_server( char *, char * );
int udp_client( char *, char *, struct sockaddr_in * );
int tselect( int, fd_set *, fd_set *, fd_set *);
unsigned int timeout( tofunc_t, void *, int );
void untimeout( unsigned int );
void init_smb( int );
void *smballoc( void );
void smbfree( void * );
void smbsend( SOCKET, void * );
void *smbrecv( SOCKET );
void set_address( char *, char *, struct sockaddr_in *, char * );
#endif  /* __ETCP_H__ */
