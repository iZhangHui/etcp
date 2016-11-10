#Uncomment the next line for SysV
#LIBS = -lsocket -lnsl -letcp -lsysv
LIBS = -letcp
MAKE = make
CC = gcc

EXECUTABLES = vrs vrc rlnumd tcpmux udpclient \
	  udpecho1 udpecho2 hello helloc udphello udphelloc tcpsource tcpsink \
	  udpsink udpsource tcprw count \
	  hb_server hb_client hb_server2 hb_client2 keep telemetrys \
	  telemetryc shutdownc tcpecho xout1 xout2 extsys \
	  tselect xout3 badserver vrcv connectto connectto1 \
	  smbs smbc endian udpconnserv badclient icmp \
	  simplec simples

.c.o :
	$(CC) -c -g -O -Wall $< -o $@

all	  : $(EXECUTABLES)

lib/libetcp.a :
	( cd lib; $(MAKE) )

vrs : vrs.o lib/libetcp.a
	$(CC) -o vrs -L lib vrs.o $(LIBS)

vrc : vrc.o lib/libetcp.a
	$(CC) -o vrc -L lib vrc.o $(LIBS)

rlnumd : rlnumd.o
	$(CC) -o rlnumd -L lib rlnumd.o $(LIBS)

tcpmux : tcpmux.o lib/libetcp.a
	$(CC) -o tcpmux -L lib tcpmux.o  $(LIBS)

udpclient : udpclient.o lib/libetcp.a
	$(CC) -o udpclient -L lib udpclient.o  $(LIBS)

udpecho1 : udpecho1.o
	$(CC) -o udpecho1 -L lib udpecho1.o $(LIBS)

udpecho2 : udpecho2.o
	$(CC) -o udpecho2 -L lib udpecho2.o $(LIBS)

hello	: hello.o
	$(CC) -o hello -L lib hello.o $(LIBS)

helloc	: helloc.o
	$(CC) -o helloc -L lib helloc.o $(LIBS)

udphello	: udphello.o
	$(CC) -o udphello -L lib udphello.o $(LIBS)

udphelloc	: udphelloc.o
	$(CC) -o udphelloc -L lib udphelloc.o $(LIBS)

tcpsource	: tcpsource.o
	$(CC) -o tcpsource -L lib tcpsource.o $(LIBS)

tcpsink	: tcpsink.o
	$(CC) -o tcpsink -L lib tcpsink.o $(LIBS)

udpsource	: udpsource.o lib/libetcp.a
	$(CC) -o udpsource -L lib udpsource.o $(LIBS)

udpsink	: udpsink.o lib/libetcp.a
	$(CC) -o udpsink -L lib udpsink.o $(LIBS)

tcprw : tcprw.o lib/libetcp.a
	$(CC) -o tcprw -L lib tcprw.o $(LIBS)

count : count.o lib/libetcp.a
	$(CC) -o count -L lib count.o $(LIBS)

hb_server : hb_server.o lib/libetcp.a
	$(CC) -o hb_server -L lib hb_server.o $(LIBS)

hb_client : hb_client.o lib/libetcp.a
	$(CC) -o hb_client -L lib hb_client.o $(LIBS)

hb_server2 : hb_server2.o lib/libetcp.a
	$(CC) -o hb_server2 -L lib hb_server2.o $(LIBS)

hb_client2 : hb_client2.o lib/libetcp.a
	$(CC) -o hb_client2 -L lib hb_client2.o $(LIBS)

keep : keep.o lib/libetcp.a
	$(CC) -o keep -L lib keep.o $(LIBS)

telemetrys : telemetrys.o lib/libetcp.a
	$(CC) -o telemetrys -L lib telemetrys.o $(LIBS)

telemetryc : telemetryc.o lib/libetcp.a
	$(CC) -o telemetryc -L lib telemetryc.o $(LIBS)

shutdownc : shutdownc.o lib/libetcp.a
	$(CC) -o shutdownc -L lib shutdownc.o $(LIBS)

tcpecho : tcpecho.o lib/libetcp.a
	$(CC) -o tcpecho -L lib tcpecho.o $(LIBS)

xout1 : xout1.o lib/libetcp.a
	$(CC) -o xout1 -L lib xout1.o $(LIBS)

xout2 : xout2.o lib/libetcp.a
	$(CC) -o xout2 -L lib xout2.o $(LIBS)

xout3 : xout3.o lib/libetcp.a tselect.o
	$(CC) -o xout3 -L lib xout3.o tselect.o $(LIBS)

extsys : extsys.o lib/libetcp.a
	$(CC) -o extsys -L lib extsys.o $(LIBS)

tselect : tselect.c lib/libetcp.a etcp.h
	$(CC) -Wall -g -o tselect -DTEST tselect.c  -L lib $(LIBS)

badserver : badserver.o lib/libetcp.a
	$(CC) -Wall -g -o badserver -L lib badserver.o  $(LIBS)

vrcv : vrcv.o lib/libetcp.a
	$(CC) -Wall -g -o vrcv -L lib vrcv.o  $(LIBS)

connectto : connectto.o
	$(CC) -Wall -g -o connectto connectto.o

connectto1 : connectto1.o
	$(CC) -Wall -g -o connectto1 connectto1.o

smbs : smbs.o smb.o lib/libetcp.a
	$(CC) -Wall -g -o smbs -L lib smbs.o smb.o $(LIBS)

smbc : smbc.o smb.o lib/libetcp.a
	$(CC) -Wall -g -o smbc -L lib smbc.o smb.o $(LIBS)

endian : endian.o
	$(CC) -Wall -g -o endian -L lib endian.o

udpconn1 : udpconn1.o lib/libetcp.a
	$(CC) -Wall -g -o udpconn1 -L lib udpconn1.o $(LIBS)

udpconn2 : udpconn2.o lib/libetcp.a
	$(CC) -Wall -g -o udpconn2 -L lib udpconn2.o $(LIBS)

udpconnserv : udpconnserv.o lib/libetcp.a
	$(CC) -Wall -g -o udpconnserv -L lib udpconnserv.o $(LIBS)

badclient : badclient.o lib/libetcp.a
	$(CC) -Wall -g -o badclient -L lib badclient.o $(LIBS)

icmp : icmp.o lib/libetcp.a
	$(CC) -Wall -g -o icmp -L lib icmp.o $(LIBS)

simplec : simplec.o
	$(CC) -Wall -g -o simplec simplec.o

simples : simples.o
	$(CC) -Wall -g -o simples simples.o

vrs.o vrc.o rlnumd.o : etcp.h
tcpmux.o udpclient.o  tcpsink.o tcpsource.o : etcp.h
udpsource.o udpsink.o keep.o: etcp.h
telemetrys.o telemetryc.o shutdownc.o tcpecho.o : etcp.h
xout1.o xout2.o xout3.o extsys.o tselect.o : etcp.h
badserver.o smb.o smbs.o smbc.o udpconn1.o udpconn2.o udpconnserv.o: etcp.h
hb_server.o hb_client.o hb_server2.o hb_client2.o: etcp.h heartbeat.h
endian.o badclient.o icmp.o : etcp.h

clean :
	-rm *.o
	-rm *.core
	-rm *.scr
	-rm $(EXECUTABLES)

cleanlib :
	( cd lib; $(MAKE) clean )

cleanall : clean cleanlib
