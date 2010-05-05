// Copyright (C) Christian Stimming, 2010

#ifndef _GPSRECEIVER_HPP
#define _GPSRECEIVER_HPP

#include <QtCore/QObject>
class PositionWGS84;

class GPSReceiver : public QObject
{
    Q_OBJECT

public:
    GPSReceiver(QObject *parent);
    virtual ~GPSReceiver();

    static GPSReceiver *createGPS_Serial(QObject *parent, const QString& comPort, int baudRate);
    static GPSReceiver *createReplay(QObject *parent, const QString& filename);

signals:
    void newPositionWGS84(const PositionWGS84& pos);

    void showMessage ( const QString & message, int timeout);
};


#endif

// Local Variables:
// indent-tabs-mode:nil
// c-basic-offset:4
// tab-width:8
// End:
