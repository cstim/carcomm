// Copyright (C) Christian Stimming, 2010

#include "nmeareplay.hpp"
#include <QtCore/QTimer>
#include <QtCore/QDebug>

NMEAReplay::NMEAReplay(QObject* parent)
    : GPSReceiver(parent)
{
    m_NMEAParser.setIsLive(false);
}
NMEAReplay::~NMEAReplay()
{}

void NMEAReplay::nextData()
{
    if (m_nextPos.isValid())
    {
        //qDebug() << "Replaying new GPS pos" << m_nextPos.toString();
        emit newPositionWGS84(m_nextPos);
    }
    PositionWGS84 next = findNext(m_stream);
    if (next.isValid())
    {
        next.setTimestamp(next.getTimestamp() + m_timeOffset);

        int millisec = (next.getTimestamp() - m_nextPos.getTimestamp()).total_milliseconds();
        //qDebug() << "Scheduling after millisec=" << millisec;
        millisec = std::max(millisec, 0);
        QTimer::singleShot(millisec,
                           this, SLOT(nextData()));
        m_nextPos = next;
    }
    else
    {
        emit showMessage(tr("GPS log reached the end of file."), 3000);
        emit showStatus(cs::BAD);
        qDebug() << "GPS log reached the end of file.";
    }
}

PositionWGS84 NMEAReplay::findNext(QTextStream& stream)
{
    PositionWGS84 p;
    while (!stream.atEnd() && !p.isValid())
    {
        QString str = stream.readLine();
        //qDebug() << "Got str" << str;
        m_NMEAParser.parseNMEAString(str.toStdString(), p);
    }
    return p;
}

bool NMEAReplay::init(const QString& filename)
{
    // Open the file
    m_file.setFileName(filename);
    bool r = m_file.open(QIODevice::ReadOnly);
    if (!r) return false;
    m_stream.setDevice(&m_file);

    // Read the first WGS84 pos
    m_nextPos = findNext(m_stream);
    if (m_nextPos.isValid())
    {
        m_timeOffset = boost::posix_time::microsec_clock::universal_time() - m_nextPos.getTimestamp();
        m_nextPos.setTimestamp(m_nextPos.getTimestamp() + m_timeOffset);
        QTimer::singleShot(0, this, SLOT(nextData()));
        return true;
    }
    else
        return false;
}

// Local Variables:
// indent-tabs-mode:nil
// c-basic-offset:4
// tab-width:8
// End:
