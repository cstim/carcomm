// Copyright (C) Christian Stimming, 2010

#ifndef _GPSREPLAY_HPP
#define _GPSREPLAY_HPP

#include "gpsreceiver.hpp"
#include "NMEAParser.hpp"
#include "PositionWGS84.hpp"
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <boost/date_time/posix_time/posix_time.hpp>

class NMEAReplay : public GPSReceiver
{
    Q_OBJECT

public:
    NMEAReplay(QObject *parent);
    virtual ~NMEAReplay();

    bool init(const QString& filename);

public slots:
    void nextData();

private:
    PositionWGS84 findNext(QTextStream& stream);

    QFile m_file;
    QTextStream m_stream;
    NMEAParser m_NMEAParser;
    PositionWGS84 m_nextPos;
    boost::posix_time::time_duration m_timeOffset;
};


#endif

// Local Variables:
// indent-tabs-mode:nil
// c-basic-offset:4
// tab-width:8
// End:
