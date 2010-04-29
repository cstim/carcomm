#define _USE_MATH_DEFINES
#include <cmath>
#include "PositionWGS84.hpp"

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
    INT16 D = INT16(Dm / 100.0);
    double m = Dm - 100.0 * D;
    return deg2rad(D + m / 60.0);
}

// Local Variables:
// indent-tabs-mode:nil
// c-basic-offset:4
// tab-width:8
// End:
