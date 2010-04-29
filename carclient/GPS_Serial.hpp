
#ifndef _API_GPS_Serial_HPP
#define _API_GPS_Serial_HPP

#include "PositionWGS84.hpp"
#include "NMEAParser.hpp"

#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/version.hpp>
#include <boost/scoped_ptr.hpp>
#if BOOST_VERSION < 103600
# error "This file compiles only with boost-1.36.0 or newer because it needs <boost/asio/serial_port.hpp>."
#endif

# include <boost/asio/serial_port.hpp>
# ifndef BOOST_ASIO_HAS_SERIAL_PORT
#  error "Oops, your boost::asio does not define BOOST_ASIO_HAS_SERIAL_PORT. This might be caused by (_WIN32_WINNT < 0x0400). Please compile this with different _WIN32_WINNT so that boost::asio has all features again."
# endif

#include <QtCore>

/**
 * \brief Receiver for data from serially connected GPS devices.
 *
 * This device opens a serial port specified by the device parameter and reads data from
 * the interface. This data will be parsed and PositionWGS84 messages will be generated and
 * will be send to the serializer.
 *
 * Currently only a NMEA-parsing will be performed.
 *
 */
class GPS_Serial : public QObject
{
    Q_OBJECT

public:
    /**
     * \brief Constructor.
     *
     * \param device_type Type name of the wanted device, e.g. "AlascaXT" or "ECU".
     * \param name The configuration name identifies an instance of
     *				 the given configuration type. It is a textual name like
     *				 "front left sensor" that facilitates reading the trace
     *				 output.
     * \param manager A reference to the Ibeo-API manager that owns this device.
     *				 The owner is responsible to create, initialize, and clean-up
     *				 its devices.
     */
    GPS_Serial();
    ~GPS_Serial();

    /**
     * \brief Initializes the device.
     */
    bool init(const std::string& comPort, UINT32 baudRate);

    /// Shuts down the device. The opposite of init().
    bool shutdown();

    /**
     * \brief Starts the device so that it emits data of type PositionWGS84.
     */
    bool run();

    /**
     * \brief Stops the device from emitting data. Opposite of run().
     */
    bool stop();

    /**
     * \brief Inidicated if the device is running or not.
     */
    bool isRunning() { return m_running; } ;

signals:
    void newPositionWGS84(const PositionWGS84& pos);

// private functions
private:
    /// The reception thread for serial GPS port
    void receptionThread();

    std::string getLongName() const { return "GPS_Serial(" + m_COMPort + ")"; }

// private members
private:
    bool m_running;

    boost::asio::io_service m_serialIoService;
    boost::asio::serial_port m_portHandle;

    std::string m_COMPort;
    UINT32 m_baudRate;

    /// Reception and write thread pointers.
    boost::scoped_ptr<boost::thread> m_receptionThreadPtr;

    /// Indicators if the threads are running.
    bool m_receptionThreadRunning;

    boost::condition m_receptionThreadCondition; ///< Condition to wait for the spawned thread to be started
    boost::mutex m_receptionThreadMutex;		///< Mutex to wait for the spawned thread to be started

    ///
    NMEAParser m_NMEAParser;
};

#endif//_API_GPS_Serial_HPP

// Local Variables:
// indent-tabs-mode:nil
// c-basic-offset:4
// tab-width:8
// End:
