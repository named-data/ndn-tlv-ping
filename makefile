CC						=       g++

CFLAGS                  =       -c -Wall

LFLAGS                  =       -lboost_system

SFLAGS                  =       /usr/local/lib/libndn-cpp-dev.so

all                     :       ndn-tlv-pingserver ndn-tlv-ping

ndn-tlv-pingserver      :       ndn-tlv-pingserver.o
                                $(CC) ndn-tlv-pingserver.o -o ndn-tlv-pingserver  $(LFLAGS) $(SFLAGS)

ndn-tlv-ping            :       ndn-tlv-ping.o
                                $(CC) ndn-tlv-ping.o -o ndn-tlv-ping  $(LFLAGS) $(SFLAGS)

ndn-tlv-pingserver.o	:   	ndn-tlv-pingserver.cpp
                        		$(CC) $(CFLAGS) ndn-tlv-pingserver.cpp  $(LFLAGS)

ndn-tlv-ping.o          :       ndn-tlv-ping.cpp
                                $(CC) $(CFLAGS) ndn-tlv-ping.cpp  $(LFLAGS)

clean                   :
                                rm -rf *.o ndn-tlv-pingserver ndn-tlv-ping

PREFIX                  =       /usr/local
BINDIR                  =       $(PREFIX)/bin

install                 :
                                install -D ndn-tlv-pingserver $(BINDIR)/ndn-tlv-pingserver
                                install -D ndn-tlv-ping $(BINDIR)/ndn-tlv-ping

uninstall               :
                                -rm $(BINDIR)/ndn-tlv-pingserver
                                -rm $(BINDIR)/ndn-tlv-ping
