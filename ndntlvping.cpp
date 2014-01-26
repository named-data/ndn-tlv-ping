/**
 *
 * Copyright (C) 2013 University of Arizona.
 * @author: Jerald Paul Abraham <jeraldabraham@email.arizona.edu>
 *
 */

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <ndn-cpp-dev/face.hpp>
#include <ndn-cpp-dev/name.hpp>
#include <ndn-cpp-dev/security/key-chain.hpp>

#if NDN_CPP_HAVE_CXX11
// In the std library, the placeholders are in a different namespace than boost.
using namespace ndn::func_lib::placeholders;
#endif

using namespace ndn;

class NdnTlvPing
{
public:

  NdnTlvPing( char* programName )
  {
    allowCaching_ = false;
    printTimestamp_ = false;
    totalPings_ = -1;
    startPingNumber_ = -1;
    pingsSent_ = 0;
    pingsReceived_ = 0;
    pingInterval_ = getPingMinimumInterval();
    clientIdentifier_ = 0;
    pingTimeoutThreshold_ = 1000;
    programName_ = programName;
    ioService_ = ptr_lib::make_shared<boost::asio::io_service>();
    face_ = Face(ioService_);
    //pingStatistics_ = new PingStatistics();
  }

  class PingStatistics
  {
  public:

	PingStatistics()
    {
		sent = 0;
		received = 0;
		startTime = boost::posix_time::second_clock::local_time();
		min = 0;
		max = 0;
		totalSum1 = 0;
		totalSum2 = 0;
    }

	int sent;
	int received;
	boost::posix_time::ptime startTime;
	double min;
	double max;
	double totalSum1;
	double totalSum2;

  };

  NdnTlvPing()
    : keyChain_()
  {
  }

  void
  usage()
  {
    std::cout << "\nUsage: " << programName_ << " ndnx:/name/prefix [options]\n"
        "Ping a NDN name prefix using Interests with name"
        "ndnx:/name/prefix/ping/number.\n"
        "The numbers in the Interests are randomly generated unless specified.\n"
        "  [-i interval] - set ping interval in seconds (minimum "
        << getPingMinimumInterval() << " second)\n"
        "  [-c count] - set total number of pings\n"
        "  [-n number] - set the starting number, the number is incremented by 1"
        " after each Interest\n"
        "  [-p identifier] - add identifier to the Interest names before the"
        " numbers to avoid conflict\n"
        "  [-a] - allow routers to return ping Data from cache"
        " (allowed by default if NDNx version < 0.8.0)\n"
        "  [-t] - print timestamp\n"
        "  [-h] - print this message and exit\n\n";
    exit(1);
  }

  double
  getPingMinimumInterval()
  {
    return 1;
  }

  void
  setTotalPings( int totalPings )
  {
    if (totalPings <= 0)
      usage();
    totalPings_ = totalPings;
  }

  void
  setPingInterval( double pingInterval )
  {
    if (pingInterval < getPingMinimumInterval())
      usage();
    pingInterval_ = pingInterval;
  }

  void
  setStartPingNumber(long int startPingNumber)
  {
    if (startPingNumber < 0)
      usage();
    startPingNumber_ = startPingNumber;
  }

  void
  setAllowCaching()
  {
    allowCaching_ = true;
  }

  void
  setPrintTimestamp()
  {
    printTimestamp_ = true;
  }

  void
  setClientIdentifier( char* clientIdentifier )
  {
     clientIdentifier_ = clientIdentifier;
     if (strlen(clientIdentifier) == 0)
       usage();
     while (*clientIdentifier != '\0') {
        if( isalnum(*clientIdentifier) == 0 )
          usage();
        clientIdentifier++;
     }
  }

  void
  setPrefix( char* prefix )
  {
    prefix_ = prefix;
  }

  void
  onData(ndn::Face &face,
	     const ndn::ptr_lib::shared_ptr<const ndn::Interest> &interest,
	     const ndn::ptr_lib::shared_ptr<ndn::Data> &data)
  {
    std::cout << "Data Received" << std::endl;
  }

  void
  onTimeout(ndn::Face &face,
          const ndn::ptr_lib::shared_ptr<const ndn::Interest> &interest)
  {
	  std::cout << "Interest Timeout Occurred" << std::endl;
  }

  void
  performPing(const boost::system::error_code& errorCode, boost::asio::deadline_timer* deadlineTimer)
  {
	if ((totalPings_ < 0) || (pingsSent_ < totalPings_)) {
      pingsSent_++;
      pingStatistics_.sent++;

      //Perform Ping
      std::cout << "Performing Ping " << pingsSent_ << std::endl;
      long int pingNumber;
      char pingNumberString[20];
      Name pingPacketName(prefix_);

      pingPacketName.append("ping");
      if(clientIdentifier_ != 0)
       	pingPacketName.append(clientIdentifier_);
      std::memset(pingNumberString, 0, 20);
      if (startPingNumber_ < 0)
    	pingNumber = (long int)std::rand();
      else
      {
    	pingNumber = startPingNumber_;
    	startPingNumber_++;
      }
      sprintf(pingNumberString, "%ld", pingNumber);
      pingPacketName.append(pingNumberString);

      ndn::Interest interest(pingPacketName);
      if (allowCaching_)
    	interest.setMustBeFresh(false);
      else
    	interest.setMustBeFresh(true);
      interest.setInterestLifetime(pingTimeoutThreshold_);
      interest.setNonce(pingNumber);

      try {
        face_.expressInterest(  interest,
    	  	  	  	  	  	    ndn::func_lib::bind(&NdnTlvPing::onData, this, boost::ref(face_), _1, _2),
      	  	  	  	  	  	    ndn::func_lib::bind(&NdnTlvPing::onTimeout, this, boost::ref(face_), _1));
        //face_.processEvents(-1);
      }
      catch(std::exception &e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        exit(1);
      }

      deadlineTimer->expires_at(  deadlineTimer->expires_at() +
    	  	                      boost::posix_time::millisec((int)(pingInterval_*1000)));

      deadlineTimer->async_wait(	boost::bind(	&NdnTlvPing::performPing,
    												this,
    												boost::asio::placeholders::error,
    												deadlineTimer));

    }
  }

  void
  signalHandler()
  {
  	std::cout << "Signal Occurred" << std::endl;
  	ioService_.reset();
  	exit(1);
  }

  void
  initialize()
  {
	boost::asio::signal_set signalSet(*ioService_, SIGINT, SIGTERM);
	signalSet.async_wait(boost::bind(&NdnTlvPing::signalHandler, this));

	boost::asio::deadline_timer deadlineTimer(	*ioService_,
												boost::posix_time::seconds(pingInterval_));

	deadlineTimer.async_wait(	boost::bind(	&NdnTlvPing::performPing,
												this,
												boost::asio::placeholders::error,
												&deadlineTimer));


	face_.processEvents();
  }

private:

  KeyChain keyChain_;
  bool allowCaching_;
  bool printTimestamp_;
  int pingTimeoutThreshold_;
  int totalPings_;
  int pingsSent_;
  int pingsReceived_;
  long int startPingNumber_;
  double pingInterval_;
  char* clientIdentifier_;
  char *programName_;
  char *prefix_;
  PingStatistics pingStatistics_;
  ptr_lib::shared_ptr<boost::asio::io_service> ioService_;
  Face face_;

};


int main(int argc, char* argv[])
{
  int res;

  try {
    NdnTlvPing ndnTlvPing (argv[0]);
    while ((res = getopt(argc, argv, "htai:c:n:p:")) != -1) {
      switch (res) {
        case 'a'  :
          ndnTlvPing.setAllowCaching();
          break;
        case 'c'  :
          ndnTlvPing.setTotalPings(atoi(optarg));
          break;
        case 'h'  :
          ndnTlvPing.usage();
          break;
        case 'i'  :
          ndnTlvPing.setPingInterval(atof(optarg));
          break;
        case 'n'  :
          ndnTlvPing.setStartPingNumber(atol(optarg));
          break;
        case 'p'  :
          ndnTlvPing.setClientIdentifier(optarg);
          break;
        case 't'  :
          ndnTlvPing.setPrintTimestamp();
          break;
        default   :
          ndnTlvPing.usage();
          break;
      }
    }

    argc -= optind;
    argv += optind;

    if (argv[0] == NULL)
      ndnTlvPing.usage();

    ndnTlvPing.setPrefix(argv[0]);

    ndnTlvPing.initialize();

  }
  catch(std::exception &e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
  return 0;
}

