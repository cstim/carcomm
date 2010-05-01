#include "carmainwindow.hpp"
#include "ui_carmainwindow.h"
#include "slicesender.hpp"


CarMainWindow::CarMainWindow()
        : base_class()
        , ui(new Ui_MainWindow())
        , m_server("http://carcomm.cstimming.de/")
//    , m_server("http://localhost:3000/")
        , m_gpsDevice()
        , m_sliceSender(new SliceSender(m_server, this))
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

    // Signal/slots of GPS Device
    connect(&m_gpsDevice, SIGNAL(newPositionWGS84(const PositionWGS84&)),
            this, SLOT(setPositionWGS84(const PositionWGS84&)));

    // Signal/slots of SliceSender
    connect(&m_gpsDevice, SIGNAL(newPositionWGS84(const PositionWGS84&)),
            m_sliceSender, SLOT(setPositionWGS84(const PositionWGS84&)));
    connect(m_sliceSender, SIGNAL(showMessage( const QString &, int)),
            statusBar(), SLOT(showMessage( const QString &, int)));
    connect(ui->actionTransmitPositionMessages, SIGNAL(triggered(bool)),
            m_sliceSender, SLOT(setAutoSendData(bool)));

}

CarMainWindow::~CarMainWindow()
{
    on_buttonConnect_clicked(false);
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
    ui->lineLatitude->setText(cs::degToString(pos.getLatitudeDeg()));
    ui->lineLongitude->setText(cs::degToString(pos.getLongitudeDeg()));
    ui->dateTimeEdit->setDateTime(pos.getQTimestamp().toLocalTime());

    // Also center the map
    QString lat = ui->lineLatitude->text();
    QString lon = ui->lineLongitude->text();
    if (lat != "nan" && lon != "nan")
    {
        ui->webView->page()->mainFrame()->evaluateJavaScript(QString("setLatLon(%1, %2);").arg(lat).arg(lon));
        reloadMapMaybe();
    }
}

void CarMainWindow::on_buttonSendData_clicked()
{
    m_sliceSender->sendDataNow();
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
    QString min_time = cs::qtDateTimeToString(currentTime.addSecs(-60 * retrieveInterval));
    QString max_time = cs::qtDateTimeToString(currentTime);
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
