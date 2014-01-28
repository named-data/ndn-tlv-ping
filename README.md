**Notes:**
---
&nbsp;&nbsp;To run the following must be ensured
<br>&nbsp;&nbsp;&nbsp;&nbsp;* Install ndnx
<br>&nbsp;&nbsp;&nbsp;&nbsp;* Install ndn-tlv
<br>&nbsp;&nbsp;&nbsp;&nbsp;* Install ndn-cpp
<br>&nbsp;&nbsp;&nbsp;&nbsp;* Install and Configure Security Environment with ndn-cpp-security-tools
<br>
**Compile And Installation Instructions:**
---
&nbsp;&nbsp;$ git clone git://github.com/jeraldabraham/ndn-tlv-ping ndntlvping
<br>&nbsp;&nbsp;$ cd ndntlvping
<br>&nbsp;&nbsp;$ make
<br>&nbsp;&nbsp;$ sudo make install
<br>
**Tool Run Instructions & Command Line Options:**
---
&nbsp;&nbsp;Usage: ndn-tlv-ping ndnx:/name/prefix [options]
<br>&nbsp;&nbsp;Ping a NDN name prefix using Interests with namendnx:/name/prefix/ping/number.
<br>&nbsp;&nbsp;The numbers in the Interests are randomly generated unless specified.
<br>&nbsp;&nbsp;&nbsp;&nbsp;[-i interval] - set ping interval in seconds (minimum 1 second)
<br>&nbsp;&nbsp;&nbsp;&nbsp;[-c count] - set total number of pings
<br>&nbsp;&nbsp;&nbsp;&nbsp;[-n number] - set the starting number, the number is incremented by 1 after each Interest
<br>&nbsp;&nbsp;&nbsp;&nbsp;[-p identifier] - add identifier to the Interest names before the numbers to avoid conflict
<br>&nbsp;&nbsp;&nbsp;&nbsp;[-a] - allow routers to return ping Data from cache (allowed by default if NDNx version < 0.8.0)
<br>&nbsp;&nbsp;&nbsp;&nbsp;[-t] - print timestamp
<br>&nbsp;&nbsp;&nbsp;&nbsp;[-h] - print this message and exit
<br>
<br>&nbsp;&nbsp;Usage: ndn-tlv-pingserver ndnx:/name/prefix [options]
<br>&nbsp;&nbsp;Starts a NDN ping server that responds to Interests with name ndnx:/name/prefix/ping/number.
<br>&nbsp;&nbsp;&nbsp;&nbsp;[-x freshness] - set FreshnessSeconds
<br>&nbsp;&nbsp;&nbsp;&nbsp;[-h] - print this message and exit

**Example:**
---
&nbsp;&nbsp;**On Machine #1: Start The ndnd-tlv**
<br>&nbsp;&nbsp;$ ndnd-tlv-start
<br>&nbsp;&nbsp;**On Machine #1: Start The Ping Server**
<br>&nbsp;&nbsp;$ ndn-tlv-pingserver /test/pingserver
<br>
<br>&nbsp;&nbsp;**On Machine #2: Start The ndnd-tlv**
<br>&nbsp;&nbsp;$ ndnd-tlv-start
<br>&nbsp;&nbsp;**On Machine #2: Start The Ping Client Program**
<br>&nbsp;&nbsp;$ ndn-tlv-ping /test/pingserver
<br>
<br>&nbsp;&nbsp;Use command line options shown above to adjust ping configuration.
