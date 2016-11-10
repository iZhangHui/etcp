#include <winsock2.h>

struct icmp
{

	unsigned char icmp_type;
	unsigned char icmp_code;
	unsigned short icmp_cksum;
};

struct ip
{
	unsigned int	ip_hl:4,
					ip_v:4;
	unsigned char	ip_tos;
	unsigned short	ip_len;
	unsigned short	ip_id;
	unsigned short	ip_off;
	unsigned short	ip_ttl;
	unsigned short	ip_p;
	unsigned short	ip_sum;
	struct in_addr	ip_src;
	struct in_addr	ip_dst;
};

struct udphdr
{
	unsigned short	uh_sport;
	unsigned short	uh_dport;
	unsigned short	uh_ulen;
	unsigned short	uh_sum;
};

#define	ICMP_MINLEN							8
#define	ICMP_ECHOREPLY						0
#define	ICMP_UNREACH						3
#define	ICMP_UNREACH_NET					0
#define	ICMP_UNREACH_HOST					1
#define	ICMP_UNREACH_PROTOCOL				2
#define	ICMP_UNREACH_PORT					3
#define	ICMP_UNREACH_NEEDFRAG				4
#define	ICMP_UNREACH_SRCFAIL				5
#define	ICMP_UNREACH_NET_UNKNOWN			6
#define	ICMP_UNREACH_HOST_UNKNOWN			7
#define	ICMP_UNREACH_ISOLATED				8
#define	ICMP_UNREACH_NET_PROHIB				9
#define	ICMP_UNREACH_HOST_PROHIB			10
#define	ICMP_UNREACH_TOSNET					11
#define	ICMP_UNREACH_TOSHOST				12
#define	ICMP_UNREACH_FILTER_PROHIB			13
#define	ICMP_UNREACH_HOST_PRECEDENCE		14
#define	ICMP_UNREACH_PRECEDENCE_CUTOFF		15
#define	ICMP_SOURCEQUENCH					4
#define	ICMP_REDIRECT						5
#define	ICMP_REDIRECT_NET					0
#define	ICMP_REDIRECT_HOST					1
#define	ICMP_REDIRECT_TOSNET				2
#define	ICMP_REDIRECT_TOSHOST				3
#define	ICMP_ECHO							8
#define	ICMP_ROUTERADVERT					9
#define	ICMP_ROUTERSOLICIT					10
#define	ICMP_TIMXCEED						11
#define	ICMP_TIMXCEED_INTRANS				0
#define	ICMP_TIMXCEED_REASS					1
#define	ICMP_PARAMPROB						12
#define	ICMP_PARAMPROB_OPTABSENT			1
#define	ICMP_TSTAMP							13
#define	ICMP_TSTAMPREPLY					14
#define	ICMP_IREQ							15
#define	ICMP_IREQREPLY						16
#define	ICMP_MASKREQ						17
#define	ICMP_MASKREPLY						18
#define	ICMP_MAXTYPE						18
