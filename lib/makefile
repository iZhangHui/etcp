CC = gcc
LIBFCNS =	error.o readcrlf.o readline.o readn.o readvrec.o set_address.o \
			tcp_client.o tcp_server.o udp_client.o udp_server.o
.c.o :
	$(CC) -c -g -O -Wall $< -o $@

all : libetcp.a libsysv.a

libetcp.a : $(LIBFCNS)
	-@rm libetcp.a
	ar -q libetcp.a $(LIBFCNS)
	ranlib libetcp.a

libsysv.a : sysv.o
	-@rm libsysv.a
	ar -q libsysv.a sysv.o
	ranlib libsysv.a

clean :
	-@rm *.o
	-@rm libetcp.a
	-@rm libsysv.a
