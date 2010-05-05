// Copyright (C) Christian Stimming, 2010

#ifndef _SLICESENDER_HPP
#define _SLICESENDER_HPP

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtNetwork/QNetworkAccessManager>

#include "PositionWGS84.hpp"

class SliceSender : public QObject
{
    Q_OBJECT
    typedef QObject base_class;

public:
    SliceSender(const QString& server, QObject *parent = NULL);

    void sendDataMaybe();
    int getSendInterval() const;

public slots:
    void setPositionWGS84(const PositionWGS84& pos);
    void setAutoSendData(bool checked) { m_autoSendData = checked; }
    //void setServer(const QString& server) { m_serverUrl = server; }
    void sendDataNow();

    void replyFinished(QNetworkReply*);

signals:
    void showMessage ( const QString & message, int timeout);

private:
    QString m_serverUrl;
    bool m_autoSendData;
    QNetworkAccessManager m_manager;
    PositionWGS84 m_currentPos;
    PositionWGS84 m_lastPos;
};

#endif

// Local Variables:
// indent-tabs-mode:nil
// c-basic-offset:4
// tab-width:8
// End:
