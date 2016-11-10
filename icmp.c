/* include header */
#include <sys/types.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include "etcp.h"

/* end header */

/* print_unreachable - print information about unreachable */
static void print_unreachable( struct icmp *icmp )
{
	struct ip *ip;
	struct udphdr *udp;
	char laddr[ 15 + 1 ];
	static char *unreach[] =
	{
		"Network unreachable",				/* 0 */
		"Host unreachable",					/* 1 */
		"Protocol unreachable",				/* 2 */
		"Port unreachable",					/* 3 */
		"Fragmentation needed, DF bit set",	/* 4 */
		"Source route failed",				/* 5 */
		"Destination network unknown",		/* 6 */
		"Destination host unknown",			/* 7 */
		"Source host isolated",				/* 8 */
		"Dest. network admin. prohibited",	/* 9 */
		"Dest. host admin. prohibited",		/* 10 */
		"Network unreachable for TOS",		/* 11 */
		"Host unreachable for TOS",			/* 12 */
		"Communication admin. prohibited",	/* 13 */
		"Host precedence violation",		/* 14 */
		"Precedence cutoff in effect"		/* 15 */
	};

	ip = ( struct ip * )( ( char * )icmp + 8 );
	udp = ( struct udphdr * )( ( char * )ip + ( ip->ip_hl << 2 ) );
	strcpy( laddr, inet_ntoa( ip->ip_src ) );
	printf( "\t%s\n\tSrc: %s.%d, Dest: %s.%d\n",
		icmp->icmp_code < ( sizeof( unreach ) /
			sizeof( unreach[ 0 ] ) )?
			unreach[ icmp->icmp_code ] : "Illegal code",
		laddr, ntohs( udp->uh_sport ),
		inet_ntoa( ip->ip_dst ), ntohs( udp->uh_dport ) );
}

/* get_type - map ICMP type to ASCII string */
static char *get_type( unsigned icmptype )
{
	static char *type[] =
	{
		"Echo Reply",						/* 0 */
		"ICMP Type 1",						/* 1 */
		"ICMP Type 2",						/* 2 */
/*@.bp*/
		"Destination Unreachable",			/* 3 */
		"Source Quench",					/* 4 */
		"Redirect",							/* 5 */
		"ICMP Type 6",						/* 6 */
		"ICMP Type 7",						/* 7 */
		"Echo Request",						/* 8 */
		"Router Advertisement",				/* 9 */
		"Router Solicitation",				/* 10 */
		"Time Exceeded",					/* 11 */
		"Parameter Problem",				/* 12 */
		"Timestamp Request",				/* 13 */
		"Timestamp Reply",					/* 14 */
		"Information Request",				/* 15 */
		"Information Reply",				/* 16 */
		"Address Mask Request",				/* 17 */
		"Address Mask Reply"				/* 18 */
	};

	if ( icmptype < ( sizeof( type ) / sizeof( type[ 0 ] ) ) )
		return type[ icmptype ];
	return "UNKNOWN";
}

/* print_dg - print an ICMP datagram */
static void print_dg( char *dg, int len )
{
	struct ip *ip;
	struct icmp *icmp;
	struct hostent *hp;
	char *hname;
	int hl;
	static char *redirect_code[] =
	{
		"network", "host",
		"type-of-service and network", "type-of-service and host"
	};
	static char *timexceed_code[] =
	{
		"transit", "reassembly"
	};
	static char *param_code[] =
	{
		"IP header bad", "Required option missing"
	};

	ip = ( struct ip * )dg;
	if ( ip->ip_v != 4 )
	{
		error( 0, 0, "IP datagram not version 4\n" );
		return;
	}
	hl = ip->ip_hl << 2;		/* IP header length in bytes */
	if ( len < hl + ICMP_MINLEN )
	{
		error( 0, 0, "short datagram (%d bytes) from %s\n",
			len, inet_ntoa( ip->ip_src ) );
		return;
	}
	hp = gethostbyaddr( ( char * )&ip->ip_src, 4, AF_INET );
	if ( hp == NULL )
		hname = "";
	else
		hname = hp->h_name;
	icmp = ( struct icmp * )( dg + hl );  /* ICMP packet */
	printf( "ICMP %s (%d) from %s (%s)\n",
		get_type( icmp->icmp_type ),
		icmp->icmp_type, hname, inet_ntoa( ip->ip_src ) );
	if ( icmp->icmp_type == ICMP_UNREACH )
		print_unreachable( icmp );
	else if ( icmp->icmp_type == ICMP_REDIRECT )
		printf( "\tRedirect for %s\n", icmp->icmp_code <= 3 ?
			redirect_code[ icmp->icmp_code ] : "Illegal code" );
	else if ( icmp->icmp_type == ICMP_TIMXCEED )
		printf( "\tTTL == 0 during %s\n", icmp->icmp_code <= 1 ?
			timexceed_code[ icmp->icmp_code ] : "Illegal code" );
/*@.bp*/
	else if ( icmp->icmp_type == ICMP_PARAMPROB )
		printf( "\t%s\n", icmp->icmp_code <= 1 ?
			param_code[ icmp->icmp_code ] : "Illegal code" );
}

/* main - initialization and event loop */
int main( int argc, char **argv )
{
	SOCKET s;
	struct protoent *pp;
	int rc;
	char icmpdg[ 1024 ];

	INIT();
	pp = getprotobyname( "icmp" );
	if ( pp == NULL )
		error( 1, errno, "getprotobyname failed" );
	s = socket( AF_INET, SOCK_RAW, pp->p_proto );
	if ( !isvalidsock( s ) )
		error( 1, errno, "socket failed" );

	for ( ;; )
	{
		rc = recvfrom( s, icmpdg, sizeof( icmpdg ), 0,
			NULL, NULL );
		if ( rc < 0 )
			error( 1, errno, "recvfrom failed" );
		print_dg( icmpdg, rc );
	}
}
