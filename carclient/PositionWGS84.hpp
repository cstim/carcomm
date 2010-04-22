#ifndef _API_POSITIONWGS84_HPP
#define _API_POSITIONWGS84_HPP

#include <cmath>
#include <boost/date_time/posix_time/posix_time.hpp>

class PositionWGS84
{
public:
	typedef signed short INT16;

	double getLatitude() const { return m_latitude; }
	double getLongitude() const { return m_longitude; }
	void setTimestamp(const boost::posix_time::ptime& v) { m_timestamp = v; }
	const boost::posix_time::ptime& getTimestamp() const { return m_timestamp; }

	void setLatitudeInNMEA(double Dm, char H)
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


	void setLongitudeInNMEA(double Dm, char H)
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

	void setLatitudeInRad(double new_latitude)
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

	void setLongitudeInRad(double new_longitude)
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

	void setCourseAngleInDeg(double val) { m_courseAngle = deg2rad(val); }

	void setAltitudeInMeterMSL(double val) { m_altitudeMSL = val; }

	static double deg2rad (double degree) { return degree * M_PI / 180.0; }

	double NMEA2rad( double Dm )
	{
		// Check this website for more information: http://home.online.no/~sigurdhu/Deg_formats.htm
		INT16 D = INT16(Dm / 100.0);
		double m = Dm - 100.0 * D;
		return deg2rad(D + m / 60.0);
	}

private:
	double m_latitude;
	double m_longitude;
	boost::posix_time::ptime m_timestamp;
	double m_courseAngle;
	double m_altitudeMSL;

};

#endif
