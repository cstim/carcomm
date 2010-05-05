// Copyright (C) Christian Stimming, 2010

#include "gpsreceiver.hpp"
#include "GPS_Serial.hpp"
#include "nmeareplay.hpp"

GPSReceiver::GPSReceiver(QObject *parent)
	: QObject(parent)
{}

GPSReceiver::~GPSReceiver()
{}

GPSReceiver *GPSReceiver::createReplay(QObject *parent, const QString& filename)
{
    NMEAReplay *gps = new NMEAReplay(parent);
    bool r = gps->init(filename);
    if (r)
        return gps;
    else
    {
        gps->showMessage(tr("Cannot start GPS replay from file %1").arg(filename), 3000);
        delete gps;
        return NULL;
    }
}

GPSReceiver *GPSReceiver::createGPS_Serial(QObject *parent, const QString& comPort, int baudRate)
{
    GPS_Serial *gps = new GPS_Serial(parent);

    bool r = gps->init(comPort.toStdString(), baudRate);
    if (r)
        r = gps->run();
    else
        gps->showMessage(tr("Cannot connect to GPS device %1; check the command line message.").arg(comPort), 10000);

    if (r)
    {
        return gps;
    }
    else
    {
        gps->showMessage(tr("Cannot receive data from the GPS device %1; check the command line message.").arg(comPort), 10000);
        gps->shutdown();
        delete gps;
        return NULL;
    }
}

// Local Variables:
// indent-tabs-mode:nil
// c-basic-offset:4
// tab-width:8
// End:
