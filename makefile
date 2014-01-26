CC				=	g++

CFLAGS				=	-c -Wall

LFLAGS				=	-lboost_system

SFLAGS				=	/usr/local/lib/libndn-cpp-dev.so

all				:	ndntlvpingserver ndntlvping

ndntlvpingserver		:	ndntlvpingserver.o
					$(CC) ndntlvpingserver.o -o ndntlvpingserver  $(LFLAGS) $(SFLAGS)
						
ndntlvping			:	ndntlvping.o
					$(CC) ndntlvping.o -o ndntlvping  $(LFLAGS) $(SFLAGS)

ndntlvpingserver.o		:	ndntlvpingserver.cpp
					$(CC) $(CFLAGS) ndntlvpingserver.cpp  $(LFLAGS)
												
ndntlvping.o			:	ndntlvping.cpp
					$(CC) $(CFLAGS) ndntlvping.cpp  $(LFLAGS)

clean				:	
					rm -rf *.o ndntlvpingserver ndntlvping
