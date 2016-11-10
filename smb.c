/* include common-header */
#include "etcp.h"

#define FREE_LIST		smbarray[ NSMB ].nexti

typedef union
{
	int nexti;
	char buf[ SMBUFSZ ];
} smb_t;
smb_t *smbarray;
/* end common-header */

#ifdef WINDOWS
/* include windows-implementation */
#define FILENAME	"./smbfile"
#define lock_buf()	if ( WaitForSingleObject( mutex, INFINITE )\
						 != WAIT_OBJECT_0 ) \
						error( 1, errno, "lock_buf failed" )
#define unlock_buf() if ( !ReleaseMutex( mutex ) )\
						error( 1, errno, "unlock_buf failed" )
HANDLE mutex;

void init_smb( int init_freelist )
{
	HANDLE hfile;
	HANDLE hmap;
	int i;

	mutex = CreateMutex( NULL, FALSE, "smbmutex" );
	if ( mutex == NULL )
		error( 1, errno, "CreateMutex failed" );
	hfile = CreateFile( FILENAME,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( hfile == INVALID_HANDLE_VALUE )
		error( 1, errno, "CreateFile failed" );
	hmap = CreateFileMapping( hfile, NULL, PAGE_READWRITE,
		0, NSMB * sizeof( smb_t ) + sizeof( int ), "smbarray" );
	smbarray = MapViewOfFile( hmap, FILE_MAP_WRITE, 0, 0, 0 );
	if ( smbarray == NULL )
		error( 1, errno, "MapViewOfFile failure" );

	if ( init_freelist )
	{
		for ( i = 0; i < NSMB - 1; i++ )
			smbarray[ i ].nexti = i + 1;
		smbarray[ NSMB - 1 ].nexti = -1;
		FREE_LIST = 0;
	}
}
/* end windows-implementation */

#else

/* include unix-implementation */
#include <sys/shm.h>
#include <sys/sem.h>
#define MUTEX_KEY		0x534d4253	/* SMBS */
#define SM_KEY			0x534d424d	/* SMBM */
#define lock_buf()		if ( semop( mutex, &lkbuf, 1 ) < 0 ) \
							error( 1, errno, "semop failed" )
#define unlock_buf()	if ( semop( mutex, &unlkbuf, 1 ) < 0 ) \
							error( 1, errno, "semop failed" )
int mutex;
struct sembuf lkbuf;
struct sembuf unlkbuf;

void init_smb( int init_freelist )
{
	union semun arg;
	int smid;
	int i;
	int rc;

	lkbuf.sem_op = -1;
	lkbuf.sem_flg = SEM_UNDO;
	unlkbuf.sem_op = 1;
	unlkbuf.sem_flg = SEM_UNDO;
	mutex = semget( MUTEX_KEY, 1,
		IPC_EXCL | IPC_CREAT | SEM_R | SEM_A );
	if ( mutex >= 0 )
	{
		arg.val = 1;
		rc = semctl( mutex, 0, SETVAL, arg );
		if ( rc < 0 )
			error( 1, errno, "semctl failed" );
	}
	else if ( errno == EEXIST )
	{
		mutex = semget( MUTEX_KEY, 1, SEM_R | SEM_A );
		if ( mutex < 0 )
			error( 1, errno, "semctl failed" );
	}
	else
		error( 1, errno, "semctl failed" );

	smid = shmget( SM_KEY, NSMB * sizeof( smb_t ) + sizeof( int ),
		SHM_R | SHM_W | IPC_CREAT );	
	if ( smid < 0 )
		error( 1, errno, "shmget failed" );
	smbarray = ( smb_t * )shmat( smid, NULL, 0 );
	if ( smbarray == ( void * )-1 )
		error( 1, errno, "shmat failed" );

	if ( init_freelist )
	{
		for ( i = 0; i < NSMB - 1; i++ )
			smbarray[ i ].nexti = i + 1;
		smbarray[ NSMB - 1 ].nexti = -1;
		FREE_LIST = 0;
	}
}
/* end unix-implementation */
#endif

/* smballoc - allocate a shared memory buffer */
void *smballoc( void )
{
	smb_t *bp;

	lock_buf();
	if ( FREE_LIST < 0 )
		error( 1, 0, "out of shared memory buffers\n" );
	bp = smbarray + FREE_LIST;
	FREE_LIST  = bp->nexti;
	unlock_buf();
	return bp;
}

/* smbfree - free a shared memory buffer */
void smbfree( void *b )
{
	smb_t *bp;

	bp = b;
	lock_buf();
	bp->nexti = FREE_LIST;
	FREE_LIST  = bp - smbarray;
	unlock_buf();
}

/* smbrecv - receive a shared memory buffer index */
void *smbrecv( SOCKET s )
{
	int index;
	int rc;

	rc = readn( s, ( char * )&index, sizeof( index ) );
	if ( rc == 0 )
		error( 1, 0, "smbrecv: peer disconnected\n" );
	else if ( rc != sizeof( index ) )
		error( 1, errno, "smbrecv: readn failure" );
	return smbarray + index;
}

/* smbsend - send a shared memory buffer index */
void smbsend( SOCKET s, void *b )
{
	int index;

	index = ( smb_t * )b - smbarray;
	if ( send( s, ( char * )&index, sizeof( index ), 0 ) < 0 )
		error( 1, errno, "smbsend: send failure" );
}
