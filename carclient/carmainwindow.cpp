#include "carmainwindow.hpp"
#include "ui_mainwindow.h"

inline QString qtDateTimeToString(const QDateTime& t)
{
    return t.toString("yyyy-MM-ddThh:mm:ssZ");
}

inline QDateTime ptime_to_qdatetime(const boost::posix_time::ptime& pt)
{
    QDate date(pt.date().year(), pt.date().month(), pt.date().day());
    QTime time(0, 0);
    time.addMSecs(pt.time_of_day().total_milliseconds());
    return QDateTime(date, time);
}

CarMainWindow::CarMainWindow()
    : base_class()
    , ui(new Ui_MainWindow())
    , m_server("http://carcomm.cstimming.de/")
                            //, m_server("http://localhost:3000/")
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

CarMainWindow::~CarMainWindow()
{}

void CarMainWindow::setPositionWGS84(const PositionWGS84& pos)
{
    if (!m_lastPos.isValid())
        m_lastPos = pos;
    m_currentPos = pos;

    ui->lineLatitude->setText(QString::number(pos.getLatitudeDeg()));
    ui->lineLongitude->setText(QString::number(pos.getLongitudeDeg()));
    ui->dateTimeEdit->setDateTime(ptime_to_qdatetime(pos.getTimestamp()));

    sendDataMaybe();
    // This also triggers reloadMapMaybe();
}

void CarMainWindow::sendDataMaybe()
{
    if (ui->checkBoxAutoSend->isChecked())
        on_buttonSendData_clicked();
}

void CarMainWindow::on_buttonSendData_clicked()
{
    qDebug() << "We are in on_buttonSendData_clicked";

    if (m_currentPos.isValid() && m_lastPos.isValid())
    {
        boost::posix_time::time_duration duration =
            m_currentPos.getTimestamp() - m_lastPos.getTimestamp();
        if (duration > boost::posix_time::seconds(30))
        {
            // Actually send the data
            QUrl url;
            QString endLat = QString::number(m_currentPos.getLatitudeDeg());
            QString startLat = QString::number(m_lastPos.getLatitudeDeg());
            QString endLon = QString::number(m_currentPos.getLongitudeDeg());
            QString startLon = QString::number(m_lastPos.getLongitudeDeg());
            QString time = ptime_to_qdatetime(m_currentPos.getTimestamp()).toString();
            QString dur = QString::number(duration.total_milliseconds() * 1e-3);

            qDebug() << "Now we must send a POST request with"
                     << "endLat=" << endLat
                     << "startLat=" << startLat
                     << "endLon=" << endLon
                     << "startLon=" << startLon
                     << "time=" << time
                     << "duration=" << dur;

            // And record the current position as last one
            m_lastPos = m_currentPos;
            m_currentPos.setTimestamp(boost::posix_time::not_a_date_time);
        }
    }

    // Also center the map
    QString lat = ui->lineLatitude->text();
    QString lon = ui->lineLongitude->text();
    ui->webView->page()->mainFrame()->evaluateJavaScript(QString("setLatLon(%1, %2);").arg(lat).arg(lon));
    reloadMapMaybe();
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
