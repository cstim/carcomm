// Copyright (C) Christian Stimming, 2010

#include "carmainwindow.hpp"
#include "ui_carmainwindow.h"
#include "slicesender.hpp"
#include "mapviewer.hpp"
#include <QtGui/QFileDialog>
#include <QtCore/QFileInfo>

CarMainWindow::CarMainWindow()
    : base_class()
    , ui(new Ui_MainWindow())
    , m_server("http://carcomm.cstimming.de/")
//    , m_server("http://localhost:3000/")
    , m_gpsDevice(NULL)
    , m_sliceSender(new SliceSender(m_server, this))
    , m_mapViewer(NULL) // must be initialized after setupUi()!
{
    ui->setupUi(this);

    m_mapViewer = new MapViewer(ui->webView, m_server, this);

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

#if 0 // 1 //defined(Q_WS_S60)
    ui->groupBoxCurrentPos->setVisible(false);
    ui->groupBoxSerial->setVisible(false);
#endif

    // Signal/slots of SliceSender
    connect(m_sliceSender, SIGNAL(showMessage( const QString &, int)),
            statusBar(), SLOT(showMessage( const QString &, int)));
    connect(ui->actionTransmitPositionMessages, SIGNAL(triggered(bool)),
            m_sliceSender, SLOT(setAutoSendData(bool)));
    m_sliceSender->setAutoSendData(ui->actionTransmitPositionMessages->isChecked());
    connect(ui->buttonSendData, SIGNAL(clicked()),
            m_sliceSender, SLOT(sendDataNow()));

    // Signal/slots of MapViewer
    connect(ui->actionAutoReloadMap, SIGNAL(triggered(bool)),
            m_mapViewer, SLOT(setAutoReloadWays(bool)));
    m_mapViewer->setAutoReloadWays(ui->actionAutoReloadMap->isChecked());
    connect(ui->buttonReload, SIGNAL(clicked()),
            m_mapViewer, SLOT(reloadWays()));
    connect(ui->actionResetMap, SIGNAL(triggered()),
            m_mapViewer, SLOT(reset()));
    on_comboBoxInterval_currentIndexChanged(ui->comboBoxInterval->currentIndex());
}

CarMainWindow::~CarMainWindow()
{
    on_buttonConnect_clicked(false);
}

void CarMainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void CarMainWindow::on_actionOpenReplay_triggered()
{
    delete m_gpsDevice;
    m_gpsDevice = NULL;

    QString filename = QFileDialog::getOpenFileName(this, tr("Select NMEA replay file"));
    if (!filename.isEmpty() && QFileInfo(filename).exists())
    {
        m_gpsDevice = GPSReceiver::createReplay(this, filename);

        if (m_gpsDevice)
        {
            // Signal/slots of GPS Device
            connect(m_gpsDevice, SIGNAL(newPositionWGS84(const PositionWGS84&)),
                    this, SLOT(setPositionWGS84(const PositionWGS84&)));
            connect(m_gpsDevice, SIGNAL(showMessage( const QString &, int)),
                    statusBar(), SLOT(showMessage( const QString &, int)));

            statusBar()->showMessage(tr("Opened GPS log file %1").arg(filename), 2000);
        }
    }
}

void CarMainWindow::on_buttonConnect_clicked(bool checked)
{
    if (checked)
    {
        delete m_gpsDevice;
        m_gpsDevice = NULL;

        // Open the device
        int baudRate = ui->comboBoxBaudrate->itemData(ui->comboBoxBaudrate->currentIndex()).toInt();
        QString comPort = ui->comboBoxPort->currentText();
        m_gpsDevice = GPSReceiver::createGPS_Serial(this, comPort, baudRate);
        if (!m_gpsDevice)
        {
            ui->buttonConnect->setChecked(false);
            return;
        }

        // Signal/slots of GPS Device
        connect(m_gpsDevice, SIGNAL(newPositionWGS84(const PositionWGS84&)),
                this, SLOT(setPositionWGS84(const PositionWGS84&)));
        connect(m_gpsDevice, SIGNAL(showMessage( const QString &, int)),
                statusBar(), SLOT(showMessage( const QString &, int)));

        statusBar()->showMessage(tr("Connected to GPS device %1").arg(comPort), 3000);
    }
    else
    {
        // Close the device
        if (m_gpsDevice)
            delete m_gpsDevice;
        m_gpsDevice = NULL;
        statusBar()->showMessage(tr("Disconnected the GPS device"), 3000);
    }
}

void CarMainWindow::setPositionWGS84(const PositionWGS84& pos)
{
    double latDeg = pos.getLatitudeDeg();
    ui->lineLatitude->setText(cs::degToString(latDeg));
    ui->lineLongitude->setText(cs::degToString(pos.getLongitudeDeg()));
    ui->dateTimeEdit->setDateTime(pos.getQTimestamp().toLocalTime());

    if (cs::isNaN(latDeg))
        statusBar()->showMessage(tr("GPS without valid position"), 700);

    m_sliceSender->setPositionWGS84(pos);
    m_mapViewer->setCenter(pos);
}

void CarMainWindow::on_comboBoxInterval_currentIndexChanged(int index)
{
    int retrieveInterval = ui->comboBoxInterval->itemData(index).toInt();
    m_mapViewer->setRetrieveInterval(retrieveInterval);
}


// Local Variables:
// indent-tabs-mode:nil
// c-basic-offset:4
// tab-width:8
// End:
