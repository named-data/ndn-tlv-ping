/**
 *
 * Copyright (C) 2013 University of Arizona.
 * @author: Jerald Paul Abraham <jeraldabraham@email.arizona.edu>
 *
 */

#include <boost/asio.hpp>

#include <ndn-cpp-dev/name.hpp>
#include <ndn-cpp-dev/face.hpp>
#include <ndn-cpp-dev/security/key-chain.hpp>

namespace ndn {

class NdnTlvPingServer
{
public:

  NdnTlvPingServer(char* programName)
    : m_programName(programName)
    , m_isPrintTimestampSet(false)
    , m_freshnessPeriod(getMinimumFreshnessPeriod())
    , m_totalPings(0)
    , m_ioService(new boost::asio::io_service)
    , m_face(m_ioService)
  {
  }

  void
  usage()
  {
    std::cout << "\n Usage:\n " << m_programName << " ndnx:/name/prefix [options]\n"
        " Starts a NDN ping server that responds to Interests with name"
        " ndnx:/name/prefix/ping/number.\n"
        "   [-x freshness] - set FreshnessSeconds\n"
        "   [-t]           - print timestamp\n"
        "   [-h]           - print this message and exit\n\n";
    exit(1);
  }

  time::milliseconds
  getMinimumFreshnessPeriod()
  {
    return time::milliseconds(1000);
  }

  void
  setFreshnessPeriod(int freshnessPeriod)
  {
    if (freshnessPeriod <= 0)
      usage();
    m_freshnessPeriod = time::milliseconds(freshnessPeriod);
  }

  void
  setPrefix(char* prefix)
  {
    m_prefix = prefix;
  }

  void
  setPrintTimestamp()
  {
    m_isPrintTimestampSet = true;
  }

  void
  onInterest(const Name& name, const Interest& interest)
  {
    Name interestName;
    interestName = interest.getName();
    if (m_name.isPrefixOf(interestName))
      {
        if (m_isPrintTimestampSet)
          std::cout << time::toIsoString(time::system_clock::now())  << " - ";
        std::cout << "Interest Received";
        std::cout << " - Ping Reference = " <<
          interestName.getSubName(interest.getName().size()-1).toUri().substr(1);
        std::cout << std::endl;
        char responseContent[] = "NDN TLV Ping Response";
        Data data(interestName);
        data.setFreshnessPeriod(m_freshnessPeriod);
        data.setContent((const uint8_t*)responseContent, sizeof(responseContent));
        m_keyChain.sign(data);
        m_face.put(data);
        m_totalPings++;
      }
  }

  void
  onRegisterFailed(const ndn::Name& prefix, const std::string& reason)
  {
    std::cerr << "ERROR: Failed to register prefix in local hub's daemon" << std::endl;
    std::cerr << "REASON: " << reason << std::endl;
    m_face.shutdown();
  }

  void
  signalHandler()
  {
    std::cout << "\n\nTotal Ping Interests Processed = " << m_totalPings <<std::endl;
    std::cout << "Shutting Down Ping Server (" << m_name.toUri() << ").\n" << std::endl;
    m_face.shutdown();
    exit(1);
  }

  void
  run()
  {
    std::cout << "\n=== Ping Server " << m_prefix <<" ===\n" << std::endl;

    boost::asio::signal_set signalSet(*m_ioService, SIGINT, SIGTERM);
    signalSet.async_wait(boost::bind(&NdnTlvPingServer::signalHandler, this));
    m_name.set(m_prefix);
    m_name.append("ping");
    m_face.setInterestFilter(m_name,
                             bind(&NdnTlvPingServer::onInterest,
                                  this, _1, _2),
                             bind(&NdnTlvPingServer::onRegisterFailed,
                                  this, _1,_2));
    try {
      m_face.processEvents();
    }
    catch(std::exception& e) {
          std::cerr << "ERROR: " << e.what() << std::endl;
    }
  }

private:

  KeyChain m_keyChain;
  bool m_isPrintTimestampSet;
  int m_totalPings;
  time::milliseconds m_freshnessPeriod;
  char* m_programName;
  char* m_prefix;
  Name m_name;
  shared_ptr<boost::asio::io_service> m_ioService;
  Face m_face;

};


}

int
main(int argc, char* argv[])
{
  int res;

  ndn::NdnTlvPingServer ndnTlvPingServer(argv[0]);
  while ((res = getopt(argc, argv, "hdtx:")) != -1)
    {
      switch (res) {
        case 'h':
          ndnTlvPingServer.usage();
          break;
        case 'x':
          ndnTlvPingServer.setFreshnessPeriod(atoi(optarg));
          break;
        case 't':
          ndnTlvPingServer.setPrintTimestamp();
          break;
        default:
          ndnTlvPingServer.usage();
          break;
     }
   }

  argc -= optind;
  argv += optind;


  if (argv[0] == 0)
    ndnTlvPingServer.usage();

  ndnTlvPingServer.setPrefix(argv[0]);
  ndnTlvPingServer.run();

  return 0;
}
