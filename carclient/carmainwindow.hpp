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

class CarMainWindow : public QMainWindow
{
    Q_OBJECT
    typedef QMainWindow base_class;

public:

    CarMainWindow();
    ~CarMainWindow();

public slots:

    void setPositionWGS84(const PositionWGS84& pos);

    void sendDataMaybe();

    void reloadMapMaybe();

    void on_buttonSendData_clicked();

    void on_buttonReload_clicked();

    void on_actionResetMap_triggered();

private:
    QSharedPointer<Ui_MainWindow> ui;
    QString m_server;
    PositionWGS84 m_currentPos;
    PositionWGS84 m_lastPos;
};


#endif

// Local Variables:
// indent-tabs-mode:nil
// c-basic-offset:4
// tab-width:8
// End:
