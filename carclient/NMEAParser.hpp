
#ifndef _API_NMEAPARSER_HPP
#define _API_NMEAPARSER_HPP

#include <string>
#include "PositionWGS84.hpp"


/**
 * \brief A parser for GPS NMEA messages.
 *
 */
class NMEAParser
{
public:
    NMEAParser();
    ~NMEAParser();

    bool parseNMEAString(const std::string &NMEAString, PositionWGS84& pos);

    void setIsLive(bool v) { m_isLive = v; }

// private members
private:
    PositionWGS84 m_positionWGS84;
    std::string m_timeStr;
    std::string m_dateStr;

    bool m_GPSValid;

    bool m_isLive;

// private functions
private:
    /// helper function to create the timestamp based on the time string received in an NMEA sentence
    boost::posix_time::ptime createTimestamp(const std::string& timeStr, const std::string& dateStr) const;

    /// parsing function for a GGA GPS string
    void parseGGA(const std::string& GGAStr);
    /// parsing function for a RMC GPS string
    void parseRMC(const std::string& RMCStr);

};	// class NMEAParser


#endif // _API_NMEAPARSER_HPP

// Local Variables:
// indent-tabs-mode:nil
// c-basic-offset:4
// tab-width:8
// End:
