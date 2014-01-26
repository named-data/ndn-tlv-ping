/**
 *
 * Copyright (C) 2013 University of Arizona.
 * @author: Jerald Paul Abraham <jeraldabraham@email.arizona.edu>
 *
 */

#include <ndn-cpp-dev/name.hpp>
#include <ndn-cpp-dev/face.hpp>
#include <ndn-cpp-dev/node.hpp>
#include <ndn-cpp-dev/security/key-chain.hpp>

#if NDN_CPP_HAVE_CXX11
// In the std library, the placeholders are in a different namespace than boost.
using namespace ndn::func_lib::placeholders;
#endif

using namespace ndn;

class NdnTlvPingServer
{
public:

  NdnTlvPingServer( char* programName )
  {
    daemonStatus_ = false;
    freshnessSeconds_ = getMinimumFreshnessSeconds();
    totalPings_ = 0;
    programName_ = programName;
    ioService_ = ptr_lib::make_shared<boost::asio::io_service>();
    face_ = Face(ioService_);
  }

  NdnTlvPingServer()
    : keyChain_()
  {
  }

  void
  usage()
  {
    std::cout << "Usage: " << programName_ << " ndnx:/name/prefix [options]\n"
        "Starts a NDN ping server that responds to Interests with name"
        " ndnx:/name/prefix/ping/number.\n"
        "  [-x freshness] - set FreshnessSeconds\n"
        "  [-d] - run server in daemon mode\n"
        "  [-h] - print this message and exit\n";
    exit(1);
  }

  int
  getMinimumFreshnessSeconds()
  {
    return 1;
  }

  void
  setFreshnessSeconds( int freshnessSeconds )
  {
    if (freshnessSeconds <= 0)
      usage();
    freshnessSeconds_ = freshnessSeconds;
  }

  void
  setDaemonStatus()
  {
    daemonStatus_ = true;
  }

  void
  setPrefix( char* prefix )
  {
    prefix_ = prefix;
  }

  void
  onInterest(const ptr_lib::shared_ptr<const Name> &name, const ptr_lib::shared_ptr<const Interest> &interest)
  {
    std::cout << "Interest Received" << std::endl;
    std::cout << interest->getName().toUri() << std::endl;
  }

  void
  onRegisterFailed(const ptr_lib::shared_ptr<const Name>&)
  {
	std::cerr << "ERROR: Failed to register prefix in local hub's daemon" << std::endl;
	face_.shutdown();
  }

  void
  signalHandler()
  {
  	std::cout << "Signal Occurred" << std::endl;
  	//ioService_.reset();
  	face_.unsetInterestFilter();
   	face_.shutdown();
   	exit(1);
  }

  void
  initialize()
  {
	boost::asio::signal_set signalSet(*ioService_, SIGINT, SIGTERM);
	signalSet.async_wait(boost::bind(&NdnTlvPingServer::signalHandler, this));

	name_.set(prefix_);
	name_.append("ping");

	face_.setInterestFilter(name,
    						func_lib::bind(&NdnTlvPingServer::onInterest, this, _1, _2),
    						func_lib::bind(&NdnTlvPingServer::onRegisterFailed, this, _1));
    face_.processEvents();
  }

private:

  KeyChain keyChain_;
  bool daemonStatus_;
  int totalPings_;
  int freshnessSeconds_;
  char *programName_;
  char *prefix_;
  Face face_;
  Name name_;
  ptr_lib::shared_ptr<boost::asio::io_service> ioService_;

};

int main(int argc, char* argv[])
{
  int res;

  try {
    NdnTlvPingServer ndnTlvPingServer (argv[0]);
    while ((res = getopt(argc, argv, "hdx:")) != -1) {
      switch (res) {
        case 'd'  :
          ndnTlvPingServer.setDaemonStatus();
          break;
        case 'h'  :
          ndnTlvPingServer.usage();
          break;
        case 'x'  :
          ndnTlvPingServer.setFreshnessSeconds(atoi(optarg));
          break;
        default   :
          ndnTlvPingServer.usage();
          break;
      }
    }

    argc -= optind;
    argv += optind;

    if (argv[0] == NULL)
      ndnTlvPingServer.usage();

    ndnTlvPingServer.setPrefix(argv[0]);

    ndnTlvPingServer.initialize();

  }
  catch(std::exception &e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
  return 0;
}
