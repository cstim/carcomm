#include <cmath>
#include "PositionWGS84.hpp"
#include <limits>
#include <boost/static_assert.hpp>

namespace cs
{
const float NaN = std::numeric_limits<float>::quiet_NaN();
BOOST_STATIC_ASSERT	(std::numeric_limits<float>::is_iec559);
BOOST_STATIC_ASSERT (std::numeric_limits<float>::has_quiet_NaN);

/**
 * NaN (Not-a-Number) is a special floating point value that is defined in the
 * floating point standard IEC 559.
 *
 * \sa http://en.wikipedia.org/wiki/IEC_559
 */
const double NaN_double = std::numeric_limits<double>::quiet_NaN();
BOOST_STATIC_ASSERT (std::numeric_limits<double>::is_iec559);
BOOST_STATIC_ASSERT (std::numeric_limits<double>::has_quiet_NaN);

QDateTime ptime_to_qdatetime(const boost::posix_time::ptime& pt)
{
    QDate date(pt.date().year(), pt.date().month(), pt.date().day());
    boost::posix_time::time_duration dur = pt.time_of_day();
    QTime time(dur.hours(), dur.minutes(), dur.seconds());
    // FIXME: Do something with the milliseconds here!
    //qDebug() << "Got date=" << date << " time=" << time << " fracsecs=" << dur.fractional_seconds();
    return QDateTime(date, time, Qt::UTC);
}

} // END namespace cs

QString PositionWGS84::toString() const
{
    return QString("(%1,%2)").arg(getLatitudeDeg())
        .arg(getLongitudeDeg());
}

QDateTime PositionWGS84::getQTimestamp() const
{
    return cs::ptime_to_qdatetime(m_timestamp);
}

void PositionWGS84::setLatitudeInNMEA(double Dm, char H)
{
    if ( H == 'S' || H == 's' )
    {
        setLatitudeInRad(-NMEA2rad( Dm ));
    }
    else
    {
        setLatitudeInRad( NMEA2rad( Dm ));
    }
}


void PositionWGS84::setLongitudeInNMEA(double Dm, char H)
{
    if ( H == 'W' || H == 'w' )
    {
        setLongitudeInRad(-NMEA2rad( Dm ));
    }
    else
    {
        setLongitudeInRad( NMEA2rad( Dm ));
    }
}

void PositionWGS84::setLatitudeInRad(double new_latitude)
{
    m_latitude = new_latitude;

    if (new_latitude < -M_PI_2 || new_latitude > M_PI_2) // note: M_PI_2 = M_PI / 2
    {
        std::cout << "PositionWGS84::setLatitude: The given latitude, "
                  << new_latitude / M_PI * 180
                  << " degree, is outside the definition range for this value which is [-90,90]. "
                  << "Ignoring this error condition here - hopefully this value isn't used anywhere."
                  << std::endl;
    }
}

void PositionWGS84::setLongitudeInRad(double new_longitude)
{
    m_longitude = new_longitude;

    if (new_longitude < -M_PI || new_longitude > M_PI)
    {
        std::cout << "PositionWGS84::setLongitude: The given longitude, "
                  << new_longitude / M_PI * 180
                  << " degree, is outside the definition range for this value which is [-180,180]. "
                  << "Ignoring this error condition here - hopefully this value isn't used anywhere."
                  << std::endl;
    }
}

double PositionWGS84::NMEA2rad( double Dm )
{
    // Check this website for more information: http://home.online.no/~sigurdhu/Deg_formats.htm
    int D = int(Dm / 100.0);
    double m = Dm - 100.0 * D;
    return deg2rad(D + m / 60.0);
}

// Local Variables:
// indent-tabs-mode:nil
// c-basic-offset:4
// tab-width:8
// End:
