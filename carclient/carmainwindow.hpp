// Copyright (C) Christian Stimming, 2010

#ifndef _CARMAINWINDOW_HPP
#define _CARMAINWINDOW_HPP

#include <QtGui/QMainWindow>
#include <QtWebKit/QWebView>
#include <QtWebKit/QWebFrame>
#include <QtCore/QSharedPointer>
#include <QtCore/QFile>
#include <QtCore/QDebug>

#include "ui_mainwindow.h"

class CarMainWindow : public QMainWindow
{
        Q_OBJECT
        typedef QMainWindow base_class;

    public:

        CarMainWindow()
            : base_class()
            , ui(new Ui_MainWindow())
        {
            ui->setupUi(this);

            QString filename = ":res/test.html";
            // ":res/streetmap.html";
            QFile f(filename);
            f.open(QIODevice::ReadOnly);
            ui->webView->setContent(f.readAll());
        }


    public slots:

        void on_buttonReloadMap_clicked()
        {
            qDebug() << "We are in on_buttonReloadMap_clicked";
            ui->webView->page()->mainFrame()->evaluateJavaScript("zoomToMuenchen();");
        }

    private:
        QSharedPointer<Ui_MainWindow> ui;
};


#endif

// Local Variables:
// indent-tabs-mode:nil
// c-basic-offset:4
// tab-width:8
// End:
