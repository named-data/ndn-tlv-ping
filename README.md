# ndn-tlv-ping: Reachability Testing Tool for NDN

`ndn-tlv-ping` is a data plane reachability testing tool for NDN network.

## Installation

Install prerequisites:

1. install [ndn-cxx](http://named-data.net/doc/ndn-cxx/current/INSTALL.html)
2. install [NFD](http://named-data.net/doc/NFD/current/getting-started.html)

Compile and install `ndn-tlv-ping`

    ./waf configure
    ./waf
    sudo ./waf install

## Usage

On machine 1,

1. start NFD
2. start ping server

        ndnpingserver /test/pingserver

On machine 2,

1. start NFD
2. configure a Route toward machine 1

        nfdc register /test/pingserver udp4://ip-address-of-machine1

3. start ping client

        ndnping /test/pingserver

`ndnpingserver` and `ndnping` support command line options to adjust their behavior.  
Execute `ndnpingserver -h` and `ndnping -h` for more details.
