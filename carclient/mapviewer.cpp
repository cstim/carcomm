// Copyright (C) Christian Stimming, 2010

#include "mapviewer.hpp"
#include "PositionWGS84.hpp"
#include <QDebug>
#include <QtCore/QTimer>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebView>

MapViewer::MapViewer(QWebView* webView, const QString& server, QObject *parent)
    : base_class(parent)
    , m_webView(webView)
    , m_serverUrl(server)
    , m_autoReloadWays(false)
{
    reset();
}

void MapViewer::reloadWaysMaybe()
{
    if (m_autoReloadWays)
        reloadWays();
}

void MapViewer::setRetrieveInterval(int value)
{
    if (value != m_retrieveInterval)
    {
        m_retrieveInterval = value;
        if (isInitialized())
        {
            m_lastLoadTime.addSecs(-100);
            reloadWays();
        }
    }
}

void MapViewer::setCenter(const PositionWGS84& pos)
{
    // Center the map
    QString lat = cs::degToString(pos.getLatitudeDeg());
    QString lon = cs::degToString(pos.getLongitudeDeg());
    if (lat != "nan" && lon != "nan" && isInitialized())
    {
        m_webView->page()->mainFrame()->evaluateJavaScript(QString("setLatLon(%1, %2);").arg(lat).arg(lon));
        reloadWaysMaybe();
    }
}

void MapViewer::reset()
{
    //QString filename = ":res/test.html";
    //QFile f(filename);
    //f.open(QIODevice::ReadOnly);
    //ui->webView->setContent(f.readAll());
    m_webView->setUrl(m_serverUrl + "streetmap.html");
    m_lastLoadTime.addSecs(-100);
    reloadWays();
}

bool MapViewer::isInitialized() const
{
    QVariant isInited = m_webView->page()->mainFrame()->evaluateJavaScript("initialized;");
    return (isInited.toInt() != 0);
}

void MapViewer::reloadWays()
{
    if (!isInitialized())
    {
        qDebug() << "HTML window is not yet initialized - cannot reload the map; waiting 1 second.";
        emit showStatus(cs::BAD);
        QTimer::singleShot(1000, this, SLOT(reloadWays()));
        return;
    }

    // FIXME: We must check not (only) for enough elapsed time since
    // last retrieval, but also whether we actually changed position,
    // i.e. we must keep a "dirty" state of the map.
    if (m_lastLoadTime.elapsed() < 5000)
    {
        //qDebug() << QString("Last ReloadWays was triggered too shortly ago (%1 s); not yet reloading ways").arg(m_lastLoadTime.elapsed() * 1e-3);
    }
    else
    {
        QDateTime currentTime = QDateTime::currentDateTime();
        QString min_time = cs::qtDateTimeToString(currentTime.addSecs(-60 * m_retrieveInterval));
        QString max_time = cs::qtDateTimeToString(currentTime);
        QString cmdString = QString("loadWays(\"%1\", \"%2\");").arg(min_time).arg(max_time);
        //qDebug() << "Now triggering ReloadWays with javascript-command:" << cmdString;
        m_webView->page()->mainFrame()->evaluateJavaScript(cmdString);
        m_lastLoadTime.start();
        emit showStatus(cs::GOOD);
    }
}


// Local Variables:
// indent-tabs-mode:nil
// c-basic-offset:4
// tab-width:8
// End:
