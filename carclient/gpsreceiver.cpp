// Copyright (C) Christian Stimming, 2010

#include "gpsreceiver.hpp"
#include "GPS_Serial.hpp"

GPSReceiver::GPSReceiver(QObject *parent)
	: QObject(parent)
{}

GPSReceiver::~GPSReceiver()
{}

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
