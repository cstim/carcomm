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
            reloadWays();
    }
}

void MapViewer::setCenter(const PositionWGS84& pos)
{
    // Center the map
    QString lat = cs::degToString(pos.getLatitudeDeg());
    QString lon = cs::degToString(pos.getLongitudeDeg());
    if (lat != "nan" && lon != "nan")
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
        qDebug() << "HTML view is not yet initialized - cannot reload the map; waiting 1 second.";
        QTimer::singleShot(1000, this, SLOT(reloadWays()));
        return;
    }

    QDateTime currentTime = QDateTime::currentDateTime();
    QString min_time = cs::qtDateTimeToString(currentTime.addSecs(-60 * m_retrieveInterval));
    QString max_time = cs::qtDateTimeToString(currentTime);
    QString cmdString = QString("loadWays(\"%1\", \"%2\");").arg(min_time).arg(max_time);
    //qDebug() << "We are in on_buttonReload_clicked:" << cmdString;
    m_webView->page()->mainFrame()->evaluateJavaScript(cmdString);
}


// Local Variables:
// indent-tabs-mode:nil
// c-basic-offset:4
// tab-width:8
// End:
