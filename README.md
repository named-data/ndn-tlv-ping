Ping Application For NDN (ndn-tlv-ping)
======================================

This is an application tool developed over ndn-cpp-dev APIs for TLV packet
based communication over NDN configured network. The NDN network has to
be first created with 'ndnx' and 'ndnd-tlv'. This is followed by environment
security setup with 'ndn-cpp-security-tools'. The application also requires
installation of ndn-cpp and CPP boost libraries. The installations need to
follow a strict order as the ndnd

## Prerequisites ##

Compiling and running ndn-traffic-generator requires the following dependencies:

1. C++ Boost Libraries version >= 1.48

    On Ubuntu 12.04:

        sudo apt-get install libboost1.48-all-dev

    On Ubuntu 13.10 and later

        sudo apt-get install libboost-all-dev

    On OSX with macports

        sudo port install boost

    On OSX with brew

        brew install boost

    On other platforms Boost Libraries can be installed from the packaged version for the
    distribution, if the version matches requirements, or compiled from source
    (http://www.boost.org)

2. ndn-cpp-dev library (https://github.com/named-data/ndn-cpp-dev)

    For detailed installation instructions refer to
    https://github.com/named-data/ndn-cpp-dev/blob/master/INSTALL.md and/or
    https://github.com/named-data/ndn-cpp-dev

3. NDN forwarding daemon (https://github.com/named-data/NFD)

-----------------------------------------------------

## 1. Compile And Installation Instructions: ##

    ./waf configure
    ./waf
    sudo ./waf install

## 2. Tool Run Instructions & Command Line Options: ##

    Usage: ndn-tlv-ping ndnx:/name/prefix [options]
    Ping a NDN name prefix using Interests with namendnx:/name/prefix/ping/number.
    The numbers in the Interests are randomly generated unless specified.
        [-i interval]   - set ping interval in seconds (minimum 1 second)
        [-c count]      - set total number of pings
        [-n number]     - set the starting number, the number is incremented by 1 after each Interest
        [-p identifier] - add identifier to the Interest names before the numbers to avoid conflict
        [-a]            - allow routers to return ping Data from cache (allowed by default if NDNx version < 0.8.0)
        [-t]            - print timestamp with messages
        [-h]            - print this message and exit


    Usage: ndn-tlv-pingserver ndnx:/name/prefix [options]
    Starts a NDN ping server that responds to Interests with name ndnx:/name/prefix/ping/number.
        [-x freshness] - set FreshnessSeconds
        [-p]           - specify number of pings to be satisfied (>=1)
        [-t]           - print timestamp wih messages
        [-h]           - print this message and exit


## 3. Sample Run Instructions ##

__ON MACHINE #1__

(NDN forwarding daemon should be running)

Start ping server

        ndnpingserver /test/pingserver

__ON MACHINE #2__

(NDN forwarding daemon should be running)

Start the ping client
        
        ndnping /test/pingserver

* Use command line options shown above to adjust ping configuration.
