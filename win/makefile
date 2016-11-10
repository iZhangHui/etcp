# nmake makefile to set up the source directory for windows

CP = copy

all : source init

source :
	$(CP) makefile.win ..\makefile
	$(CP) skel.h ..

init :
	-mkdir ..\sys
	-mkdir ..\netinet
	-mkdir ..\arpa
	$(CP) unistd.h ..
	$(CP) ip.h ..\netinet
	$(CP) nf ..\netdb.h
	$(CP) nf ..\sys\socket.h
	$(CP) nf ..\sys\wait.h
	$(CP) nf ..\sys\time.h
	$(CP) nf ..\sys\resource.h
	$(CP) nf ..\netinet\in.h
	$(CP) nf ..\netinet\in_systm.h
	$(CP) nf ..\netinet\ip_icmp.h
	$(CP) nf ..\netinet\udp.h
	$(CP) nf ..\arpa\inet.h

