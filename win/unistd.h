/* fake unistd.h to supply getopt definitions */
int optind;
int opterr;
int optopt;
char *optarg;
int getopt( int, char **, char * );
