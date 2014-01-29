CC			=	g++
CFLAGS			=	-c -Wall
LFLAGS			=	/usr/local/lib/libndn-cpp-dev.so /usr/lib/libboost_system.so.*
all			:	ndn-tlv-pingserver ndn-tlv-ping
ndn-tlv-pingserver	:	ndn-tlv-pingserver.o
				$(CC) ndn-tlv-pingserver.o -o ndn-tlv-pingserver  $(LFLAGS)
ndn-tlv-ping		:	ndn-tlv-ping.o
				$(CC) ndn-tlv-ping.o -o ndn-tlv-ping  $(LFLAGS)
ndn-tlv-pingserver.o	:	ndn-tlv-pingserver.cpp
				$(CC) $(CFLAGS) ndn-tlv-pingserver.cpp

ndn-tlv-ping.o		:	ndn-tlv-ping.cpp
				$(CC) $(CFLAGS) ndn-tlv-ping.cpp
clean			:
				rm -f *.o ndn-tlv-pingserver ndn-tlv-ping
PREFIX			=	/usr/local
BINDIR			=	$(PREFIX)/bin
install			:
				install -D ndn-tlv-pingserver $(BINDIR)/ndn-tlv-pingserver
				install -D ndn-tlv-ping $(BINDIR)/ndn-tlv-ping
uninstall		:
				rm -f $(BINDIR)/ndn-tlv-pingserver
				rm -f $(BINDIR)/ndn-tlv-ping
