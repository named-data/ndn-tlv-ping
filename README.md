Notes:
******
 - To run the following must be ensured
	- Install ndnx
	- Install ndn-tlv
	- Install ndn-cpp
	- Install and Configure Security Environment with ndn-cpp-security-tools

Compile And Installation Instructions:
**************************************
 git clone git://github.com/jeraldabraham/ndn-tlv-ping ndntlvping
 cd ndntlvping
 make
 sudo make install

Tool Run Instructions & Command Line Options:
*********************************************
 Usage: ndn-tlv-ping ndnx:/name/prefix [options]
 Ping a NDN name prefix using Interests with namendnx:/name/prefix/ping/number.
 The numbers in the Interests are randomly generated unless specified.
   [-i interval] - set ping interval in seconds (minimum 1 second)
   [-c count] - set total number of pings
   [-n number] - set the starting number, the number is incremented by 1 after each Interest
   [-p identifier] - add identifier to the Interest names before the numbers to avoid conflict
   [-a] - allow routers to return ping Data from cache (allowed by default if NDNx version < 0.8.0)
   [-t] - print timestamp
   [-h] - print this message and exit


 Usage: ndn-tlv-pingserver ndnx:/name/prefix [options]
 Starts a NDN ping server that responds to Interests with name ndnx:/name/prefix/ping/number.
   [-x freshness] - set FreshnessSeconds
   [-h] - print this message and exit

Example:
********
 On Machine #1: Start The Ping Server
 ndn-tlv-pingserver /test/pingserver

 On Machine #2: Start The Ping Client Program
 ndn-tlv-ping /test/pingserver

 Use command line options shown above to adjust ping configuration.

