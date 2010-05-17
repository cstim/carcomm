// Copyright (C) Christian Stimming, 2010

#ifndef _MAPVIEWER_HPP
#define _MAPVIEWER_HPP

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QTime>
class QWebView;
class PositionWGS84;

#include "PositionWGS84.hpp"

class MapViewer : public QObject
{
    Q_OBJECT
    typedef QObject base_class;
public:
    MapViewer(QWebView* webView, const QString& server, QObject *parent = NULL);
    void reloadWaysMaybe();

    bool isInitialized() const;

public slots:

    void setCenter(const PositionWGS84& pos);
    void reset();
    void reloadWays();
    void setAutoReloadWays(bool checked) { m_autoReloadWays = checked; }
    void setRetrieveInterval(int value);
    //void setServer(const QString& server) { m_serverUrl = server; }

signals:
    void showStatus(cs::Status status);

private:
    QWebView * m_webView;
    QString m_serverUrl;
    bool m_autoReloadWays;
    int m_retrieveInterval;
    QTime m_lastLoadTime;
};

#endif

// Local Variables:
// indent-tabs-mode:nil
// c-basic-offset:4
// tab-width:8
// End:
