/**
 *
 * Copyright (C) 2013 University of Arizona.
 * @author: Jerald Paul Abraham <jeraldabraham@email.arizona.edu>
 *
 */

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/noncopyable.hpp>

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/name.hpp>

namespace ndn {

class NdnTlvPing : boost::noncopyable
{
public:

  explicit
  NdnTlvPing(char* programName)
    : m_programName(programName)
    , m_isAllowCachingSet(false)
    , m_isPrintTimestampSet(false)
    , m_hasError(false)
    , m_totalPings(-1)
    , m_startPingNumber(-1)
    , m_pingsSent(0)
    , m_pingsReceived(0)
    , m_pingInterval(getPingMinimumInterval())
    , m_clientIdentifier(0)
    , m_pingTimeoutThreshold(getPingTimeoutThreshold())
    , m_ioService(new boost::asio::io_service)
    , m_face(m_ioService)
  {
  }

  class PingStatistics : boost::noncopyable
  {
  public:

    explicit
    PingStatistics()
      : m_sentPings(0)
      , m_receivedPings(0)
      , m_pingStartTime(time::steady_clock::now())
      , m_minimumRoundTripTime(std::numeric_limits<double>::max())
      , m_averageRoundTripTimeData(0)
      , m_standardDeviationRoundTripTimeData(0)
    {
    }

    void
    addToPingStatistics(time::nanoseconds roundTripNanoseconds)
    {
      double roundTripTime = roundTripNanoseconds.count() / 1000000.0;
      if (roundTripTime < m_minimumRoundTripTime)
        m_minimumRoundTripTime = roundTripTime;
      if (roundTripTime > m_maximumRoundTripTime)
        m_maximumRoundTripTime = roundTripTime;
      m_averageRoundTripTimeData += roundTripTime;
      m_standardDeviationRoundTripTimeData += roundTripTime*roundTripTime;
    }

    int m_sentPings;
    int m_receivedPings;
    time::steady_clock::TimePoint m_pingStartTime;
    double m_minimumRoundTripTime;
    double m_maximumRoundTripTime;
    double m_averageRoundTripTimeData;
    double m_standardDeviationRoundTripTimeData;

  };

  void
  usage()
  {
    std::cout << "\n Usage:\n " << m_programName << " ndn:/name/prefix [options]\n"
        " Ping a NDN name prefix using Interests with name"
        "ndn:/name/prefix/ping/number.\n"
        " The numbers in the Interests are randomly generated unless specified.\n"
        "   [-i interval]   - set ping interval in seconds (minimum "
        << getPingMinimumInterval().count() << " milliseconds)\n"
        "   [-c count]      - set total number of pings\n"
        "   [-n number]     - set the starting number, the number is incremented by 1"
        " after each Interest\n"
        "   [-p identifier] - add identifier to the Interest names before the"
        " numbers to avoid conflict\n"
        "   [-a]            - allow routers to return ping Data from cache"
        " (allowed by default if NDNx version < 0.8.0)\n"
        "   [-t]            - print timestamp with messages\n"
        "   [-h]            - print this message and exit\n\n";
    exit(1);
  }

  time::milliseconds
  getPingMinimumInterval()
  {
    return time::milliseconds(1000);
  }

  time::milliseconds
  getPingTimeoutThreshold()
  {
    return time::milliseconds(4000);
  }

  void
  setTotalPings(int totalPings)
  {
    if (totalPings <= 0)
      usage();
    m_totalPings = totalPings;
  }

  void
  setPingInterval(int pingInterval)
  {
    if (pingInterval < getPingMinimumInterval().count())
      usage();
    m_pingInterval = time::milliseconds(pingInterval);
  }

  void
  setStartPingNumber(int startPingNumber)
  {
    if (startPingNumber < 0)
      usage();
    m_startPingNumber = startPingNumber;
  }

  void
  setAllowCaching()
  {
    m_isAllowCachingSet = true;
  }

  void
  setPrintTimestamp()
  {
    m_isPrintTimestampSet = true;
  }

  void
  setClientIdentifier(char* clientIdentifier)
  {
    m_clientIdentifier = clientIdentifier;
    if (strlen(clientIdentifier) == 0)
      usage();
    while (*clientIdentifier != '\0') {
      if( isalnum(*clientIdentifier) == 0 )
        usage();
      clientIdentifier++;
    }
  }

  void
  setPrefix(char* prefix)
  {
    m_prefix = prefix;
  }

  bool
  hasError() const
  {
    return m_hasError;
  }


  void
  onData(const ndn::Interest& interest,
         Data& data,
         time::steady_clock::TimePoint timePoint)
  {
    std::string pingReference;
    time::nanoseconds roundTripTime;
    pingReference = interest.getName().toUri();
    m_pingsReceived++;
    m_pingStatistics.m_receivedPings++;
    roundTripTime = time::steady_clock::now() - timePoint;
    if (m_isPrintTimestampSet)
      std::cout << time::toIsoString(time::system_clock::now())  << " - ";
    std::cout << "Content From " << m_prefix;
    std::cout << " - Ping Reference = " <<
      interest.getName().getSubName(interest.getName().size()-1).toUri().substr(1);
    std::cout << "  \t- Round Trip Time = " <<
      roundTripTime.count() / 1000000.0 << " ms" << std::endl;
    m_pingStatistics.addToPingStatistics(roundTripTime);
  }

  void
  onTimeout(const ndn::Interest& interest)
  {
    if (m_isPrintTimestampSet)
      std::cout << time::toIsoString(time::system_clock::now())  << " - ";
    std::cout << "Timeout From " << m_prefix;
    std::cout << " - Ping Reference = " <<
      interest.getName().getSubName(interest.getName().size()-1).toUri().substr(1);
    std::cout << std::endl;
  }

  void
  printPingStatistics()
  {
    std::cout << "\n\n=== " << " Ping Statistics For "<< m_prefix <<" ===" << std::endl;
    std::cout << "Sent=" << m_pingStatistics.m_sentPings;
    std::cout << ", Received=" << m_pingStatistics.m_receivedPings;
    double packetLossRate = m_pingStatistics.m_sentPings - m_pingStatistics.m_receivedPings;
    packetLossRate /= m_pingStatistics.m_sentPings;
    std::cout << ", Packet Loss=" << packetLossRate * 100.0 << "%";
    if (m_pingStatistics.m_sentPings != m_pingStatistics.m_receivedPings)
      m_hasError = true;
    std::cout << ", Total Time=" << m_pingStatistics.m_averageRoundTripTimeData << " ms\n";
    if (m_pingStatistics.m_receivedPings > 0) {
      double averageRoundTripTime =
        m_pingStatistics.m_averageRoundTripTimeData / m_pingStatistics.m_receivedPings;
      double standardDeviationRoundTripTime =
        m_pingStatistics.m_standardDeviationRoundTripTimeData / m_pingStatistics.m_receivedPings;
      standardDeviationRoundTripTime -= averageRoundTripTime * averageRoundTripTime;
      standardDeviationRoundTripTime = std::sqrt(standardDeviationRoundTripTime);
      std::cout << "Round Trip Time (Min/Max/Avg/MDev) = (";
      std::cout << m_pingStatistics.m_minimumRoundTripTime << "/";
      std::cout << m_pingStatistics.m_maximumRoundTripTime << "/";
      std::cout << averageRoundTripTime << "/";
      std::cout << standardDeviationRoundTripTime << ") ms\n";
    }
    std::cout << std::endl;
  }

  void
  performPing(boost::asio::deadline_timer* deadlineTimer)
  {
    if ((m_totalPings < 0) || (m_pingsSent < m_totalPings))
      {
        m_pingsSent++;
        m_pingStatistics.m_sentPings++;

        //Perform Ping
        char pingNumberString[20];
        Name pingPacketName(m_prefix);
        pingPacketName.append("ping");
        if(m_clientIdentifier != 0)
          pingPacketName.append(m_clientIdentifier);
        std::memset(pingNumberString, 0, 20);
        if (m_startPingNumber < 0)
            m_startPingNumber = std::rand();
        sprintf(pingNumberString, "%d", m_startPingNumber);
        pingPacketName.append(pingNumberString);
        ndn::Interest interest(pingPacketName);
        if (m_isAllowCachingSet)
          interest.setMustBeFresh(false);
        else
          interest.setMustBeFresh(true);
        interest.setInterestLifetime(m_pingTimeoutThreshold);
        interest.setNonce(m_startPingNumber);
        m_startPingNumber++;
         try {
          m_face.expressInterest(interest,
                                 bind(&NdnTlvPing::onData,
                                      this, _1, _2,
                                      time::steady_clock::now()),
                                 bind(&NdnTlvPing::onTimeout,
                                      this, _1));
          deadlineTimer->expires_at(deadlineTimer->expires_at() +
                                    boost::posix_time::millisec(m_pingInterval.count()));
          deadlineTimer->async_wait(bind(&NdnTlvPing::performPing,
                                         this,
                                         deadlineTimer));
        }
        catch(std::exception& e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
        }
    }
    else
      {
        m_face.shutdown();
        printPingStatistics();
        m_ioService->stop();
      }
  }

  void
  signalHandler()
  {
    m_face.shutdown();
    printPingStatistics();
    exit(1);
  }

  void
  run()
  {
    std::cout << "\n=== Pinging " << m_prefix  << " ===\n" <<std::endl;

    boost::asio::signal_set signalSet(*m_ioService, SIGINT, SIGTERM);
    signalSet.async_wait(bind(&NdnTlvPing::signalHandler, this));

    boost::asio::deadline_timer deadlineTimer(*m_ioService,
                                              boost::posix_time::millisec(0));

    deadlineTimer.async_wait(bind(&NdnTlvPing::performPing,
                                  this,
                                  &deadlineTimer));
    try {
      m_face.processEvents();
    }
    catch(std::exception& e) {
      std::cerr << "ERROR: " << e.what() << std::endl;
      m_hasError = true;
      m_ioService->stop();
    }
  }

private:

  bool m_isAllowCachingSet;
  bool m_hasError;
  bool m_isPrintTimestampSet;
  time::milliseconds m_pingTimeoutThreshold;
  int m_totalPings;
  int m_pingsSent;
  int m_pingsReceived;
  int m_startPingNumber;
  time::milliseconds m_pingInterval;
  char* m_clientIdentifier;
  char* m_programName;
  char* m_prefix;
  PingStatistics m_pingStatistics;
  shared_ptr<boost::asio::io_service> m_ioService;
  Face m_face;

};

}


int
main(int argc, char* argv[])
{
  int res;

  ndn::NdnTlvPing ndnTlvPing(argv[0]);
  while ((res = getopt(argc, argv, "htai:c:n:p:")) != -1)
    {
      switch (res) {
      case 'a':
        ndnTlvPing.setAllowCaching();
        break;
      case 'c':
        ndnTlvPing.setTotalPings(atoi(optarg));
        break;
      case 'h':
        ndnTlvPing.usage();
        break;
      case 'i':
        ndnTlvPing.setPingInterval(atoi(optarg));
        break;
      case 'n':
        ndnTlvPing.setStartPingNumber(atoi(optarg));
        break;
      case 'p':
        ndnTlvPing.setClientIdentifier(optarg);
        break;
      case 't':
        ndnTlvPing.setPrintTimestamp();
        break;
      default:
        ndnTlvPing.usage();
        break;
      }
    }

  argc -= optind;
  argv += optind;

  if (argv[0] == 0)
    ndnTlvPing.usage();

  ndnTlvPing.setPrefix(argv[0]);
  ndnTlvPing.run();

  std::cout << std::endl;

  if (ndnTlvPing.hasError())
    return 1;
  else
    return 0;
}


