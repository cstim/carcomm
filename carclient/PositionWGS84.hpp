#ifndef _API_POSITIONWGS84_HPP
#define _API_POSITIONWGS84_HPP

#define _USE_MATH_DEFINES
#include <cmath>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/cstdint.hpp>
#include <boost/static_assert.hpp>

#include <QtCore/QMetaType>

typedef boost::  int8_t			  INT8;		///<  8 bit wide   signed integer
typedef boost:: uint8_t			 UINT8;		///<  8 bit wide unsigned integer
typedef boost:: int16_t			 INT16;		///< 16 bit wide   signed integer
typedef boost::uint16_t			UINT16;		///< 16 bit wide unsigned integer
#ifndef _MSC_VER // _BASETSD_H_		// avoid naming conflict with Windows <basetsd.h>
typedef boost:: int32_t			 INT32;		///< 32 bit wide   signed integer
typedef boost::uint32_t			UINT32;		///< 32 bit wide unsigned integer
typedef boost:: int64_t			 INT64;		///< 64 bit wide   signed integer
typedef boost::uint64_t			UINT64;		///< 64 bit wide unsigned integer
#endif


namespace cs
{
extern const float NaN;
extern const double NaN_double;
template<typename floatT>
inline bool isNaN (floatT x)
{
    return (x != x);
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
