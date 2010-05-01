// Copyright (C) Christian Stimming, 2010

#ifndef _CARMAINWINDOW_HPP
#define _CARMAINWINDOW_HPP

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QSharedPointer>
#include <QtCore/QTimer>
#include <QtGui/QMainWindow>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebView>

class Ui_MainWindow;
#include "PositionWGS84.hpp"
#include "GPS_Serial.hpp"
class SliceSender;

class CarMainWindow : public QMainWindow
{
    Q_OBJECT
    typedef QMainWindow base_class;

public:

    CarMainWindow();
    ~CarMainWindow();

public slots:

    void setPositionWGS84(const PositionWGS84& pos);

    void reloadMapMaybe();

    void on_buttonSendData_clicked();

    void on_buttonReload_clicked();

    void on_actionResetMap_triggered();

    void on_buttonConnect_clicked(bool checked);

private:

    QSharedPointer<Ui_MainWindow> ui;
    QString m_server;
    GPS_Serial m_gpsDevice;
    SliceSender* m_sliceSender;
};


#endif

// Local Variables:
// indent-tabs-mode:nil
// c-basic-offset:4
// tab-width:8
// End:
