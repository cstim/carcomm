#include "carmainwindow.hpp"
#include "ui_mainwindow.h"
#include <QNetworkReply>
#include <QNetworkRequest>

inline QString qtDateTimeToString(const QDateTime& t)
{
    return t.toString("yyyy-MM-ddThh:mm:ssZ");
}

inline QDateTime ptime_to_qdatetime(const boost::posix_time::ptime& pt)
{
    QDate date(pt.date().year(), pt.date().month(), pt.date().day());
    boost::posix_time::time_duration dur = pt.time_of_day();
    QTime time(dur.hours(), dur.minutes(), dur.seconds());
    //qDebug() << "Got date=" << date << " time=" << time << " fracsecs=" << dur.fractional_seconds();
    return QDateTime(date, time, Qt::UTC);
}

QString degToString(double deg)
{
    return QString::number(deg, 'f', 8);
}

CarMainWindow::CarMainWindow()
        : base_class()
        , ui(new Ui_MainWindow())
        , m_server("http://carcomm.cstimming.de/")
//    , m_server("http://localhost:3000/")
        , m_gpsDevice()
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
    comboBox->setCurrentIndex(comboBox->findText("1 Week"));

    // Fill the list of COM ports
    comboBox = ui->comboBoxPort;
#ifdef Q_OS_WIN32
    for (int i = 1; i <= 8; ++i)
        comboBox->addItem("COM" + QString::number(i));
    comboBox->setCurrentIndex(comboBox->findText("COM4"));
#else
    comboBox->addItem("/dev/ttyACM0");
    for (int i = 0; i <= 4; ++i)
        comboBox->addItem("/dev/ttyUSB" + QString::number(i));
    comboBox->setCurrentIndex(comboBox->findText("/dev/ttyUSB0"));
#endif

    // Fill the list of baud rates
    comboBox = ui->comboBoxBaudrate;
    comboBox->addItem("1200", QVariant(1200));
    comboBox->addItem("2400", QVariant(2400));
    comboBox->addItem("4800", QVariant(4800));
    comboBox->addItem("9600", QVariant(9600));
    comboBox->addItem("14400", QVariant(14400));
    comboBox->addItem("38400", QVariant(38400));
    comboBox->addItem("57600", QVariant(57600));
    comboBox->addItem("115200", QVariant(115200));
    comboBox->setCurrentIndex(comboBox->findData(QVariant(38400)));

    // Read the HTML page and write it into the webview widget
    on_actionResetMap_triggered();

    connect(&m_gpsDevice, SIGNAL(newPositionWGS84(const PositionWGS84&)), this, SLOT(setPositionWGS84(const PositionWGS84&)));

    connect(&m_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

}

CarMainWindow::~CarMainWindow()
{
    on_buttonConnect_clicked(false);
}

int CarMainWindow::getSendInterval() const
{
    return 60;
}

void CarMainWindow::on_buttonConnect_clicked(bool checked)
{
    if (checked)
    {
        // Open the device
        int baudRate = ui->comboBoxBaudrate->itemData(ui->comboBoxBaudrate->currentIndex()).toInt();
        QString comPort = ui->comboBoxPort->currentText();
        bool r = m_gpsDevice.init(comPort.toStdString(), baudRate);
        if (!r)
        {
            statusBar()->showMessage(tr("Cannot connect to GPS device %1; check the command line message.").arg(comPort), 10000);
            ui->buttonConnect->setChecked(false);
            m_gpsDevice.shutdown();
            return;
        }
        r = m_gpsDevice.run();
        if (!r)
        {
            statusBar()->showMessage(tr("Cannot receive data from the GPS device %1; check the command line message.").arg(comPort), 10000);
            ui->buttonConnect->setChecked(false);
            m_gpsDevice.shutdown();
        }
        statusBar()->showMessage(tr("Connected to GPS device %1").arg(comPort), 3000);
    }
    else
    {
        // Close the device
        if (m_gpsDevice.isRunning())
            m_gpsDevice.stop();
        m_gpsDevice.shutdown();
        statusBar()->showMessage(tr("Disconnected the GPS device"), 3000);
    }
}

void CarMainWindow::setPositionWGS84(const PositionWGS84& pos)
{
    if (!m_lastPos.isValid() || cs::isNaN(m_lastPos.getLatitudeDeg()))
        m_lastPos = pos;
    m_currentPos = pos;

    ui->lineLatitude->setText(degToString(pos.getLatitudeDeg()));
    ui->lineLongitude->setText(degToString(pos.getLongitudeDeg()));
    ui->dateTimeEdit->setDateTime(ptime_to_qdatetime(pos.getTimestamp()).toLocalTime());

    sendDataMaybe();
    // This also triggers reloadMapMaybe();
}

void CarMainWindow::sendDataMaybe()
{
    if (ui->checkBoxAutoSend->isChecked()
        && m_currentPos.isValid()
        && m_lastPos.isValid())
    {
        boost::posix_time::time_duration duration =
            m_currentPos.getTimestamp() - m_lastPos.getTimestamp();
        if (duration > boost::posix_time::seconds(getSendInterval()))
            on_buttonSendData_clicked();
    }
}

void CarMainWindow::on_buttonSendData_clicked()
{
    //qDebug() << "We are in on_buttonSendData_clicked";

    if (m_currentPos.isValid()
        && m_lastPos.isValid()
        && !cs::isNaN(m_currentPos.getLatitudeDeg())
        && !cs::isNaN(m_lastPos.getLatitudeDeg())
        && !cs::isNaN(m_currentPos.getLongitudeDeg())
        && !cs::isNaN(m_lastPos.getLongitudeDeg())
       )
    {
        boost::posix_time::time_duration duration =
            m_currentPos.getTimestamp() - m_lastPos.getTimestamp();
        //if (duration > boost::posix_time::seconds(getSendInterval()))
        {
            // Actually send the data
            QUrl url;
            QString endLat = degToString(m_currentPos.getLatitudeDeg());
            QString startLat = degToString(m_lastPos.getLatitudeDeg());
            QString endLon = degToString(m_currentPos.getLongitudeDeg());
            QString startLon = degToString(m_lastPos.getLongitudeDeg());
			QString time = qtDateTimeToString(ptime_to_qdatetime(m_currentPos.getTimestamp()).toUTC());
            QString dur = QString::number(duration.total_milliseconds() * 1e-3);

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
            request.setUrl(QUrl(m_server + "slices"));
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
            m_manager.post(request, form.toLocal8Bit());

            // And record the current position as last one
            m_lastPos = m_currentPos;
            m_currentPos.setTimestamp(boost::posix_time::not_a_date_time);
        }
    }
    else
    {
        statusBar()->showMessage(tr("Cannot send position message - Latitude/Longitude not received."), 3000);
    }

    // Also center the map
    QString lat = ui->lineLatitude->text();
    QString lon = ui->lineLongitude->text();
    if (lat != "nan" && lon != "nan")
    {
        ui->webView->page()->mainFrame()->evaluateJavaScript(QString("setLatLon(%1, %2);").arg(lat).arg(lon));
        reloadMapMaybe();
    }
}

void CarMainWindow::replyFinished(QNetworkReply*  reply)
{
    qDebug() << "Reply had this error:" << reply->errorString();
    if (reply->error() == QNetworkReply::NoError)
    {
        statusBar()->showMessage(tr("Position message sent successfully."), 3000);
    }
    else
    {
        statusBar()->showMessage(tr("There was an error sending the position:") + reply->errorString(), 10000);
    }
    reply->deleteLater();
}

void CarMainWindow::reloadMapMaybe()
{
    if (ui->checkBoxAutoReload->isChecked())
        on_buttonReload_clicked();
}

void CarMainWindow::on_buttonReload_clicked()
{
    QVariant isInited = ui->webView->page()->mainFrame()->evaluateJavaScript("initialized;");
    if (isInited.toInt() == 0)
    {
        qDebug() << "HTML view is not yet initialized - cannot reload the map; waiting 1 second.";
        QTimer::singleShot(1000, this, SLOT(on_buttonReload_clicked()));
        return;
    }

    QDateTime currentTime = ui->dateTimeEdit->dateTime();
    int retrieveInterval = ui->comboBoxInterval->itemData(ui->comboBoxInterval->currentIndex()).toInt();
    QString min_time = qtDateTimeToString(currentTime.addSecs(-60 * retrieveInterval));
    QString max_time = qtDateTimeToString(currentTime);
    QString cmdString = QString("loadWays(\"%1\", \"%2\");").arg(min_time).arg(max_time);
    //qDebug() << "We are in on_buttonReload_clicked:" << cmdString;
    ui->webView->page()->mainFrame()->evaluateJavaScript(cmdString);
}

void CarMainWindow::on_actionResetMap_triggered()
{
    //QString filename = ":res/test.html";
    //QFile f(filename);
    //f.open(QIODevice::ReadOnly);
    //ui->webView->setContent(f.readAll());
    ui->webView->setUrl(m_server + "streetmap.html");
    on_buttonReload_clicked();
}

// Local Variables:
// indent-tabs-mode:nil
// c-basic-offset:4
// tab-width:8
// End:
