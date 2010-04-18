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

#include "ui_mainwindow.h"

inline QString qtDateTimeToString(const QDateTime& t)
{
    return t.toString("yyyy-MM-ddThh:mm:ssZ");
}

class CarMainWindow : public QMainWindow
{
        Q_OBJECT
        typedef QMainWindow base_class;

    public:

        CarMainWindow()
            : base_class()
            , ui(new Ui_MainWindow())
//             , m_server("http://carcomm.cstimming.de/")
            , m_server("http://localhost:3000/")
        {
            ui->setupUi(this);

            // Set the date to now
            ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());

            // Fill the list of available time intervals
            QComboBox* comboBox = ui->comboBoxInterval;
            comboBox->addItem("1 Year", QVariant(525600));
            comboBox->addItem("6 Weeks", QVariant(60480));
            comboBox->addItem("1 Week", QVariant(10080));
            comboBox->addItem("1 Day", QVariant(1440));
            comboBox->addItem("2 Hours", QVariant(120));
            comboBox->addItem("15 Minutes", QVariant(15));
            comboBox->addItem("5 Minutes", QVariant(5));

            // Read the HTML page and write it into the webview widget
            on_actionResetMap_triggered();
        }


    public slots:

        void sendDataMaybe()
        {
            if (ui->checkBoxAutoSend->isChecked())
                on_buttonSendData_clicked();
        }

        void reloadMapMaybe()
        {
            if (ui->checkBoxAutoReload->isChecked())
                on_buttonReload_clicked();
        }

        void on_buttonSendData_clicked()
        {
            qDebug() << "We are in on_buttonSendData_clicked";
            // Add Implementation!
            QString lat = ui->lineLatitude->text();
            QString lon = ui->lineLongitude->text();
            ui->webView->page()->mainFrame()->evaluateJavaScript(QString("setLatLon(%1, %2);").arg(lat).arg(lon));
            reloadMapMaybe();
        }

        void on_buttonReload_clicked()
        {
            QVariant isInited = ui->webView->page()->mainFrame()->evaluateJavaScript("initialized;");
            if (isInited.toInt() == 0)
            {
                qDebug() << "HTML view is not yet initialized - cannot reload the map; waiting 0.5 seconds.";
                QTimer::singleShot(500, this, SLOT(on_buttonReload_clicked()));
                return;
            }

            QDateTime currentTime = ui->dateTimeEdit->dateTime();
            int interval = ui->comboBoxInterval->itemData(ui->comboBoxInterval->currentIndex()).toInt();
            QString min_time = qtDateTimeToString(currentTime.addSecs(-60 * interval));
            QString max_time = qtDateTimeToString(currentTime);
            QString cmdString = QString("loadWays(\"%1\", \"%2\");").arg(min_time).arg(max_time);
            qDebug() << "We are in on_buttonReload_clicked:" << cmdString;
            ui->webView->page()->mainFrame()->evaluateJavaScript(cmdString);
        }

        void on_actionResetMap_triggered()
        {
            //QString filename = ":res/test.html";
            //QFile f(filename);
            //f.open(QIODevice::ReadOnly);
            //ui->webView->setContent(f.readAll());
            ui->webView->setUrl(m_server + "streetmap.html");
            on_buttonReload_clicked();
        }

    private:
        QSharedPointer<Ui_MainWindow> ui;
        QString m_server;
};


#endif

// Local Variables:
// indent-tabs-mode:nil
// c-basic-offset:4
// tab-width:8
// End:
