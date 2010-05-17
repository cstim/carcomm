#include "NMEAParser.hpp"
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include <QtCore>

NMEAParser::NMEAParser()
        : m_positionWGS84()
        , m_timeStr("")
        , m_dateStr("")
        , m_GPSValid(false)
        , m_isLive(true)
{
}

NMEAParser::~NMEAParser()
{
}

bool NMEAParser::parseNMEAString(const std::string &NMEAString, PositionWGS84& pos)
{
    if (NMEAString.length() >= 6)
    {
        // Get the header of the GPSDataString
        const std::string header(NMEAString.substr(3, 3));

        m_GPSValid = false;

        if (header == "RMC")	// includes position, time, course Angle
        {
            parseRMC(NMEAString);
        }
        if (header == "GGA")	// includes altitude and GPS type
        {
            parseGGA(NMEAString);
        }

        // if a valid GPS signal is available send the internal WGS84 position
        // otherwise return an empty position
        if (m_GPSValid)
        {
            m_positionWGS84.setTimestamp(createTimestamp(m_timeStr, m_dateStr));
            pos = m_positionWGS84;
            return true;
        }
        else
        {
            //std::cout << "parseNMEA: GPS is not yet complete" << std::endl;
        }
    }
    else
    {
        std::cout << "parseNMEA: Ignoring too short string, \"" << NMEAString << "\"" << std::endl;
    }

    return false;
}

boost::posix_time::ptime NMEAParser::createTimestamp(const std::string& timeStr, const std::string& dateStr) const
{
    // create timestamp
    try
    {
        const int hours(boost::lexical_cast<int> (timeStr.substr(0, 2)));
        const int minutes(boost::lexical_cast<int> (timeStr.substr(2, 2)));
        const int seconds(boost::lexical_cast<int> (timeStr.substr(4, 2)));
        const int milliseconds(boost::lexical_cast<int> (timeStr.substr(7, 3)));

        const boost::posix_time::time_duration time(boost::posix_time::hours(hours) +
                boost::posix_time::minutes(minutes) +
                boost::posix_time::seconds(seconds) +
                boost::posix_time::milliseconds(milliseconds));

        boost::gregorian::date date;

        if (dateStr.size() == 6)
        {
            const int day(boost::lexical_cast<int> (dateStr.substr(0, 2)));
            const int month(boost::lexical_cast<int> (dateStr.substr(2, 2)));
            const int year(2000 + boost::lexical_cast<int> (dateStr.substr(4, 2)));

            date = boost::gregorian::date (year, month, day);
        }
        else
        {
            if (m_isLive)
                date = boost::gregorian::day_clock::universal_day();
            else
                return boost::posix_time::ptime();
        }

        const boost::posix_time::ptime timestamp(boost::posix_time::ptime(date) + time);

        return timestamp;
    }
    catch (const boost::bad_lexical_cast &)
    {
        return boost::posix_time::ptime();
    }
}

void NMEAParser::parseRMC(const std::string& RMCStr)
{
    //RMC - Recommended Minimum Navigation Information

    //                                                           12
    //       1         2 3       4 5        6 7   8   9    10  11|
    //       |         | |       | |        | |   |   |    |   | |
    //$--RMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,xxxx,x.x,a*hh<CR><LF>

    //Field Number:
    // 1) UTC Time
    // 2) Status, V = Navigation receiver warning
    // 3) Latitude
    // 4) N or S
    // 5) Longitude
    // 6) E or W
    // 7) Speed over ground, knots
    // 8) Track made good, degrees true
    // 9) Date, ddmmyy
    //10) Magnetic Variation, degrees
    //11) E or W
    //12) Checksum

    // Local storage for NMEA numbers
    double latitude(cs::NaN_double);
    double longitude(cs::NaN_double);
    double courseAngle(cs::NaN_double);
    char northSouth = 'N'; // Default set to northern hemisphere (positive sign in latitude)
    char eastWest = 'E'; // Default set to east (positive sign in longitude)

    // Seperate GPS data from string incoming string
    boost::tokenizer<boost::escaped_list_separator<char> > tokensOfGPSData (RMCStr);
    boost::tokenizer<boost::escaped_list_separator<char> >::const_iterator currentGPSData;

    int counter(0);
    for (currentGPSData = tokensOfGPSData.begin(); currentGPSData != tokensOfGPSData.end(); currentGPSData++)
    {
        if (*currentGPSData != "")
        {
            switch (counter)
            {
            case 0:		// 0) $GPRMC
                break;

            case 1:		// 1) Universal Time Coordinated (UTC)
                m_timeStr = (*currentGPSData);
                break;

            case 2:		// 2) Status, V = Navigation receiver warning
                if (*currentGPSData == "V")
                {
                    return;
                }
                break;

            case 3:		// 3) Latitude
                try
                {
                    latitude = boost::lexical_cast<double> (*currentGPSData);
                }
                catch (const boost::bad_lexical_cast &)
                {
                    latitude = cs::NaN_double;
                }
                break;

            case 4:		// 4) N or S (North or South)
                northSouth = currentGPSData->at(0);
                break;

            case 5:		// 5) Longitude
                try
                {
                    longitude = boost::lexical_cast<double> (*currentGPSData);
                }
                catch (const boost::bad_lexical_cast &)
                {
                    longitude = cs::NaN_double;
                }
                break;

            case 6:		// 6) E or W (East or West)
                eastWest = currentGPSData->at(0);
                break;

            case 7:		// 7) Speed over ground, knots
                break;

            case 8:		// 8) Track made good, degrees true
                try
                {
                    courseAngle = -1.0 * boost::lexical_cast<double> (*currentGPSData);
                }
                catch (const boost::bad_lexical_cast &)
                {
                    courseAngle = cs::NaN_double;
                }
                break;

            case 9:		// 9) Date, ddmmyy
                m_dateStr = (*currentGPSData);
                break;

            case 10:	//10) Magnetic Variation, degrees
                break;

            case 11:	//11) E or W
                break;

            case 12:	//12) Checksum
                break;

            default:
                break;
            }
        }

        counter++;
    }

    // fill local WGS84 position with new values if we have a valid GPS position
    m_positionWGS84.setLatitudeInNMEA(latitude, northSouth);
    m_positionWGS84.setLongitudeInNMEA(longitude, eastWest);
    m_positionWGS84.setCourseAngleInDeg(courseAngle);

    // We only *require* RMC message to be present. If we are here,
    // all GPS data has been read to generate a qualified
    // PositionWGS84 message. So we set the status to valid.
    m_GPSValid = true;
}

void NMEAParser::parseGGA(const std::string& GGAStr)
{
    //GGA - Global Positioning System Fix Data, Time, Position and fix related data fora GPS receiver.

    //                                                     11
    //       1         2       3 4        5 6 7  8   9  10 |  12 13  14   15
    //       |         |       | |        | | |  |   |   | |   | |   |    |
    //$--GGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh<CR><LF>

    //Field Number:
    // 1) Universal Time Coordinated (UTC)
    // 2) Latitude
    // 3) N or S (North or South)
    // 4) Longitude
    // 5) E or W (East or West)
    // 6) GPS Quality Indicator,
    //    0 - fix not available,
    //    1 - GPS fix,
    //    2 - Differential GPS fix
    // 7) Number of satellites in view, 00 - 12
    // 8) Horizontal Dilution of precision
    // 9) Antenna Altitude above/below mean-sea-level (geoid)
    //10) Units of antenna altitude, meters
    //11) Geoidal separation, the difference between the WGS-84 earth
    //    ellipsoid and mean-sea-level (geoid), "-" means mean-sea-level
    //    below ellipsoid
    //12) Units of geoidal separation, meters
    //13) Age of differential GPS data, time in seconds since last SC104
    //    type 1 or 9 update, null field when DGPS is not used
    //14) Differential reference station ID, 0000-1023
    //15) Checksum

    double latitude(cs::NaN_double);
    double longitude(cs::NaN_double);
    char northSouth = 'N'; // Default set to northern hemisphere (positive sign in latitude)
    char eastWest = 'E'; // Default set to east (positive sign in longitude)
    double altitude(cs::NaN_double);

    // Seperate GPS data from string incoming string
    boost::tokenizer<boost::escaped_list_separator<char> > tokensOfGPSData (GGAStr);
    boost::tokenizer<boost::escaped_list_separator<char> >::const_iterator currentGPSData;

    int counter(0);
    for (currentGPSData = tokensOfGPSData.begin(); currentGPSData != tokensOfGPSData.end(); currentGPSData++)
    {
        if (*currentGPSData != "")
        {
            switch (counter)
            {
            case 0:		// 0) $GPGGA
                break;

            case 1:		// 1) Universal Time Coordinated (UTC)
                m_timeStr = (*currentGPSData);
                break;

            case 2:		// 2) Latitude
                try
                {
                    latitude = boost::lexical_cast<double> (*currentGPSData);
                }
                catch (const boost::bad_lexical_cast &)
                {
                    latitude = cs::NaN;
                }
                break;

            case 3:		// 3) N or S (North or South)
                northSouth = currentGPSData->at(0);
                break;

            case 4:		// 4) Longitude
                try
                {
                    longitude = boost::lexical_cast<double> (*currentGPSData);
                }
                catch (const boost::bad_lexical_cast &)
                {
                    longitude = cs::NaN;
                }
                break;

            case 5:		// 5) E or W (East or West)
                eastWest = currentGPSData->at(0);
                break;

            case 6:		// 6) GPS Quality Indicator, 0 - fix not available, 1 - GPS fix, 2 - Differential GPS fix
                if (*currentGPSData == "1")
                {
                    //sourceType = PositionWGS84::GPS_SPS;
                }
                else if (*currentGPSData == "2")
                {
                    //sourceType = PositionWGS84::GPS_SBAS;
                }
                else
                {
                    //sourceType = PositionWGS84::Unknown;
                }
                break;

            case 7:		// 7) Number of satellites in view, 00 - 12
                break;

            case 8:		// 8) Horizontal Dilution of precision
                break;

            case 9:		// 9) Antenna Altitude above/below mean-sea-level (geoid)
                try
                {
                    altitude = boost::lexical_cast<double> (*currentGPSData);
                }
                catch (const boost::bad_lexical_cast &)
                {
                    altitude = cs::NaN_double;
                }
                break;

            case 10:	//10) Units of antenna altitude, meters
                break;

            case 11:	//11) Geoidal separation
                break;

            case 12:	//12) Units of geoidal separation, meters
                break;

            case 13:	//13) Age of differential GPS data
                break;

            case 14:	//14) Differential reference station ID, 0000-1023
                break;

            case 15:	//15) Checksum
                break;

            default:
                break;
            }
        }

        counter++;
    }

    // fill local WGS84 position with new values if we have a valid GPS position
    m_positionWGS84.setLatitudeInNMEA(latitude, northSouth);
    m_positionWGS84.setLongitudeInNMEA(longitude, eastWest);
    m_positionWGS84.setAltitudeInMeterMSL(altitude);

    m_GPSValid = true;
}

// Local Variables:
// indent-tabs-mode:nil
// c-basic-offset:4
// tab-width:8
// End:
