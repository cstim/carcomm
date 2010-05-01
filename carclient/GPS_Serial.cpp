#include "GPS_Serial.hpp"

#include <boost/bind.hpp>

# include <boost/asio/streambuf.hpp>
# include <boost/asio/read_until.hpp>


GPS_Serial::GPS_Serial()
        : m_running(false)
        , m_serialIoService()
        , m_portHandle(m_serialIoService)
        , m_COMPort()
        , m_baudRate()
        , m_receptionThreadPtr()
        , m_receptionThreadRunning(false)
        , m_NMEAParser()
{
    //define (new ParamString   ("Device", m_COMPort, "/dev/ttyACM0"));
    //define (new ParamBaudRate ("BaudRate", m_baudRate, "Baud rate of serial port (1200 Baud, 2400 Baud, 9600 Baud, 14400 Baud, 38400 Baud, 115200 Baud"));

}

GPS_Serial::~GPS_Serial(void)
{
    // Stop the device
    if (isRunning())
        stop();

    shutdown();
}

bool GPS_Serial::shutdown()
{
    // close serial port
    m_portHandle.close();
    return true;
}

bool GPS_Serial::init(const std::string& comPort, int baudRate)
{
    m_COMPort = comPort;
    m_baudRate = baudRate;

    {
        std::cout << "Initializing GPS receiver..." << std::endl;
    }

    {
        std::cout << "Opening serial connection..." << std::endl;
    }

    // Open COM port
    boost::system::error_code ec;
    ec = m_portHandle.open(m_COMPort, ec);
    if (ec.value() != 0)
    {
        std::cout << "Error: Could not open serial port " << m_COMPort << std::endl;

        return false;
    }

    ec = m_portHandle.set_option( boost::asio::serial_port_base::baud_rate(m_baudRate), ec);
    if (ec.value() != 0)
    {
        std::cout << "Error: Could not set baudrate." << std::endl;

        return false;
    }

    return true;
}

bool GPS_Serial::run()
{
    if (isRunning())
    {
        std::cout << "Duplicate RUN command: " << getLongName() << " is running already. Ignoring command." << std::endl;
        return true;
    }

    {
        boost::mutex::scoped_lock lock(m_receptionThreadMutex);
        m_receptionThreadRunning = false;

        m_receptionThreadPtr.reset(new boost::thread (boost::bind (&GPS_Serial::receptionThread, this)));

        // Make sure the other thread really started up before
        // continuing. (Otherwise the destructor might be called before
        // the thread was really started, which is bad.)
        while (m_receptionThreadRunning == false)
        {
            m_receptionThreadCondition.wait(lock);
        }
    }

    if (!m_receptionThreadPtr)
    {
        std::cout << "Error: Creating GPS reception thread failed." << std::endl;
        return false;
    }

    m_running = true;

    return true;
}

bool GPS_Serial::stop()
{
    if (!isRunning())
    {
        std::cout << "Duplicate STOP command: " << getLongName() << " is stopped already. Ignoring command." << std::endl;
        return true;
    }

    {
        boost::mutex::scoped_lock lock(m_receptionThreadMutex);
        m_receptionThreadRunning = false;
    }

    if (m_receptionThreadPtr)
    {
        {
            std::cout << "Try to stop GPS receiption thread." << std::endl;
        }
        m_receptionThreadPtr->join();
        m_receptionThreadPtr.reset(NULL);
    }

    {
        std::cout << "GPS reception thread stopped." << std::endl;
    }

    m_running = false;

    return true;
}

void GPS_Serial::receptionThread()
{
    {
        boost::mutex::scoped_lock lock(m_receptionThreadMutex);
        m_receptionThreadRunning = true;
        m_receptionThreadCondition.notify_all();
    }

    {
        std::cout << "Reception thread started for " << getLongName() << std::endl;
    }

    PositionWGS84 pos;

    // Main loop of serial data reception
    while (m_receptionThreadRunning)
    {
        boost::asio::streambuf b;
        boost::asio::read_until(m_portHandle, b, "\n");
        std::istream is(&b);
        std::string gpsStr;
        std::getline(is, gpsStr);

        // parse now the received GPS string and signal the position
        if (m_NMEAParser.parseNMEAString(gpsStr, pos))
        {
            emit newPositionWGS84(pos);
        }
    }

    {
        std::cout << "Reception thread terminating for " << getLongName() << std::endl;
    }
}

// Local Variables:
// indent-tabs-mode:nil
// c-basic-offset:4
// tab-width:8
// End:
