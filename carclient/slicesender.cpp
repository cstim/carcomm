// Copyright (C) Christian Stimming, 2010

#include "slicesender.hpp"
#include <QDebug>
#include <QNetworkReply>
#include <QNetworkRequest>

SliceSender::SliceSender(const QString& server, QObject *parent)
    : base_class(parent)
    , m_serverUrl(server)
    , m_autoSendData(false)
    , m_manager()
    , m_currentPos()
    , m_lastPos()
{
    connect(&m_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}

int SliceSender::getSendInterval() const
{
    return 60;
}

void SliceSender::setPositionWGS84(const PositionWGS84& pos)
{
    if (!m_lastPos.isValid() || cs::isNaN(m_lastPos.getLatitudeDeg()))
        m_lastPos = pos;
    m_currentPos = pos;

    sendDataMaybe();
}

void SliceSender::sendDataMaybe()
{
    if (m_autoSendData
        && m_currentPos.isValid()
        && m_lastPos.isValid())
    {
        int secs = m_lastPos.getQTimestamp().secsTo(m_currentPos.getQTimestamp());
        if (secs >= getSendInterval())
            sendDataNow();
    }
}

void SliceSender::sendDataNow()
{
    qDebug() << "We are in sendDataNow";

    if (m_currentPos.isValid()
        && m_lastPos.isValid()
        && !cs::isNaN(m_currentPos.getLatitudeDeg())
        && !cs::isNaN(m_lastPos.getLatitudeDeg())
        && !cs::isNaN(m_currentPos.getLongitudeDeg())
        && !cs::isNaN(m_lastPos.getLongitudeDeg())
       )
    {
        int duration_secs = m_lastPos.getQTimestamp().secsTo(m_currentPos.getQTimestamp());
        //if (secs >= getSendInterval())
        {
            // Actually send the data
            QUrl url;
            QString endLat = cs::degToString(m_currentPos.getLatitudeDeg());
            QString startLat = cs::degToString(m_lastPos.getLatitudeDeg());
            QString endLon = cs::degToString(m_currentPos.getLongitudeDeg());
            QString startLon = cs::degToString(m_lastPos.getLongitudeDeg());
            QString time = cs::qtDateTimeToString(m_currentPos.getQTimestamp().toUTC());
            QString dur = QString::number(duration_secs);

            QString form;
            form += "slice[lat]=" + endLat
                    + "&slice[lon]=" + endLon
                    + "&slice[time]=" + time
                    + "&slice[duration]=" + dur
                    + "&slice[startlat]=" + startLat
                    + "&slice[startlon]=" + startLon;

            qDebug() << "Now we will send a POST request with"
//                      << "endLat=" << endLat
//                      << "startLat=" << startLat
//                      << "endLon=" << endLon
//                      << "startLon=" << startLon
//                      << "time=" << time
//                      << "duration=" << dur
                     << "form=" << form;

            QNetworkRequest request;
            request.setUrl(QUrl(m_serverUrl + "slices"));
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
            m_manager.post(request, form.toLocal8Bit());

            // And record the current position as last one
            m_lastPos = m_currentPos;
            m_currentPos.setTimestamp(boost::posix_time::not_a_date_time);
        }
    }
    else
    {
        emit showMessage(tr("Cannot send position message - Latitude/Longitude not received."), 3000);
    }
}

void SliceSender::replyFinished(QNetworkReply* reply)
{
    qDebug() << "Reply had this error:" << reply->errorString();
    if (reply->error() == QNetworkReply::NoError)
    {
        emit showMessage(tr("Position message sent successfully."), 3000);
    }
    else
    {
        emit showMessage(tr("There was an error sending the position:") + reply->errorString(), 10000);
    }
    reply->deleteLater();
}


// Local Variables:
// indent-tabs-mode:nil
// c-basic-offset:4
// tab-width:8
// End:
