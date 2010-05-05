#ifndef _API_POSITIONWGS84_HPP
#define _API_POSITIONWGS84_HPP

#include <cmath>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <QtCore/QMetaType>
#include <QtCore/QDateTime>
#include <QtCore/QString>

namespace cs
{
extern const float NaN;
extern const double NaN_double;
template<typename floatT>
inline bool isNaN (floatT x)
{
    return (x != x);
}

QDateTime ptime_to_qdatetime(const boost::posix_time::ptime& pt);

inline QString degToString(double deg)
{
    return QString::number(deg, 'f', 8);
}

inline QString qtDateTimeToString(const QDateTime& t)
{
    return t.toString("yyyy-MM-ddThh:mm:ssZ");
}

} // END namespace cs


class PositionWGS84
{
public:

    PositionWGS84()
            : m_timestamp(boost::posix_time::not_a_date_time)
    {}

    bool isValid() const { return !m_timestamp.is_not_a_date_time(); }

    double getLatitudeRad() const { return m_latitude; }
    double getLongitudeRad() const { return m_longitude; }
    double getLatitudeDeg() const { return rad2deg(m_latitude); }
    double getLongitudeDeg() const { return rad2deg(m_longitude); }

    void setTimestamp(const boost::posix_time::ptime& v) { m_timestamp = v; }
    const boost::posix_time::ptime& getTimestamp() const { return m_timestamp; }
    QDateTime getQTimestamp() const;

    void setLatitudeInNMEA(double Dm, char H);

    void setLongitudeInNMEA(double Dm, char H);

    void setLatitudeInRad(double new_latitude);

    void setLongitudeInRad(double new_longitude);

    void setCourseAngleInDeg(double val) { m_courseAngle = deg2rad(val); }

    void setAltitudeInMeterMSL(double val) { m_altitudeMSL = val; }

    static double deg2rad (double degree) { return degree * M_PI / 180.0; }
    static double rad2deg (double rad) { return rad * 180.0 / M_PI; }

    static double NMEA2rad( double Dm );

private:
    double m_latitude;
    double m_longitude;
    boost::posix_time::ptime m_timestamp;
    double m_courseAngle;
    double m_altitudeMSL;

};

Q_DECLARE_METATYPE(PositionWGS84)

#endif

// Local Variables:
// indent-tabs-mode:nil
// c-basic-offset:4
// tab-width:8
// End:
