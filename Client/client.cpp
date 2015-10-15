#include <QDebug>
#include <QHostInfo>

#include "client.h"

Q_LOGGING_CATEGORY(CLIENT, "CLIENT")

Client::Client(QObject *parent): QObject(parent), m_totalBytes(0)
{
// Initialize variables and connections
    m_sendSocket = new QTcpSocket(this);
    m_receiveSocket = new QTcpSocket(this);

    readSettings();
}

Client::~Client()
{
}

// Start to listen
void Client::listen()
{
    if (m_receiveIpAddress.isEmpty())
    {
        m_receiveIpAddress = getLocalIP();
        updateSettings();
        qDebug() << "m_receiveIpAddress:" << m_receiveIpAddress;
    }

    QHostAddress ipAddress(m_receiveIpAddress.toInt());
    if(!m_server.listen(ipAddress, m_receivePort))
    {
        qCWarning(CLIENT()) << CLIENT().categoryName() << m_server.errorString();
        m_server.close();
        return;
    }
    qDebug() << "Listen OK";

    connect(&m_server, SIGNAL(newConnection()) ,
            this, SLOT(acceptConnection()));    // Send newConnection() signal when a new connection is detected
}

void Client::readSettings()
{
    QSettings *settings = new QSettings(SETTINGS_PATH, QSettings::IniFormat);
    m_receiveIpAddress = settings->value("Receive/IpAddress").toString();
    m_receivePort = settings->value("Receive/Port").toString().toUShort(0,10);
    m_sendIpAddress = settings->value("Send/IpAddress").toString();
    m_sendPort = settings->value("Send/Port").toString().toUShort(0,10);
    delete settings;
}

void Client::updateSettings()
{
    QSettings *settings = new QSettings(SETTINGS_PATH, QSettings::IniFormat);
    settings->setValue("Receive/IpAddress", m_receiveIpAddress);
    settings->setValue("Receive/Port", m_receivePort);
    settings->setValue("Send/IpAddress", m_sendIpAddress);
    settings->setValue("Send/Port", m_sendPort);
    delete settings;
}

// Accept new connection
void Client::acceptConnection()
{
    m_receiveSocket = m_server.nextPendingConnection();
    connect(m_receiveSocket, SIGNAL(readyRead()), this, SLOT(readHeader()));
    connect(m_receiveSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));

    qDebug() << "Accept connection OK";
}

// Get local IP address
QString Client::getLocalIP()
{
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address() &&
            (ipAddressesList.at(i).toString().indexOf("168") != (-1))) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    return ipAddress;
}

// Error display
void Client::displayError(QAbstractSocket::SocketError)
{
    qCWarning(CLIENT()) << CLIENT().categoryName() << ":" << m_server.errorString();
}

void Client::readHeader()
{
    QDataStream in(m_receiveSocket);
    in.setVersion(QDataStream::Qt_4_6);

//  test the signal of readyRead
//    qDebug() << "Ready read...";
    qint64 header;
    qCDebug(CLIENT()) << CLIENT().categoryName() << "Reading header...";

    in >> header;

    switch (header) {
    case COMMAND:
        receiveCommand();
        break;
    case PLAN:
        receivePlan();
        break;
    default:
        break;
    }
}

// Variables initialization
void Client::initVar()
{
    m_hashX.clear();
    m_hashY.clear();
    m_hashZ.clear();
    m_spot3D.clear();
    m_spotOrder.clear();
    m_parameter.volt = 0;
    m_parameter.totalTime = 0;
    m_parameter.period = 0;
    m_parameter.dutyCycle = 0;
    m_parameter.coolingTime = 0;
    m_baOut.clear();
}

// Receive treatment plan from another computer
void Client::receivePlan()
{
    initVar();

    QByteArray baBuffer = m_receiveSocket->readAll();
//    QDataStream in(m_receiveSocket);
    QDataStream in(baBuffer);
    in.setVersion(QDataStream::Qt_4_6);

    qCDebug(CLIENT()) << CLIENT().categoryName() << "Receiving plan...";
    QString receipt;

    in >> m_totalBytes
       >> m_hashX
       >> m_hashY
       >> m_hashZ
       >> m_spotOrder
       >> m_parameter.volt
       >> m_parameter.totalTime
       >> m_parameter.period
       >> m_parameter.dutyCycle
       >> m_parameter.coolingTime
       >> receipt;

    qDebug() << "m_totalBytes:" << m_totalBytes;
    qDebug() << "m_hashX:" << m_hashX;
    qDebug() << "m_hashY:" << m_hashY;
    qDebug() << "m_hashZ:" << m_hashZ;
    qDebug() << "m_spotOrder:" << m_spotOrder;
    qDebug() << "Volt:" << m_parameter.volt << "Total time:" << m_parameter.totalTime << "Period:" << m_parameter.period
             << "Duty cycle:" << m_parameter.dutyCycle << "Cooling time:" << m_parameter.coolingTime;
    qDebug() << "receipt:" << receipt;

    qCDebug(CLIENT()) << CLIENT().categoryName() << "Receiving plan finished.";

    QDataStream out(&m_baOut, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    qCDebug(CLIENT()) << CLIENT().categoryName() << "Sending receipt...";

    out << receipt;

//  If need to print bytesWritten information, un-comment the line below
//    connect(m_tcpClientConnection, SIGNAL(bytesWritten(qint64)), this, SLOT(bytes(qint64)));
    m_receiveSocket->write(m_baOut);

    m_receiveSocket->close();

    qDebug() << "Send receipt finished.";
    qDebug() << SEPERATOR;

    convertSpot();

    qCDebug(CLIENT()) << CLIENT().categoryName() << "RECEIVING TREATMENT PLAN SUCCEEDED.";
    qDebug() << SEPERATOR;
    emit receivingCompleted();
}

// Convert the spots information to standard form
void Client::convertSpot()
{
    QHash<float, QList<Coordinate> >::iterator i;
    for (i = m_hashX.begin(); i != m_hashX.end(); ++i)
    {
        float currentKey = i.key();
        QList<Coordinate> currentListX = i.value();
        QList<Coordinate> currentListY = m_hashY[currentKey];
        QList<Coordinate> currentListZ = m_hashZ[currentKey];
        int currentlistSize = currentListX.size();
        for (int x = 0; x < currentlistSize; x++)
        {
            Spot3DCoordinate currentSpot;
            currentSpot.x = currentListX.at(x);
            currentSpot.y = currentListY.at(x);
            currentSpot.z = currentListZ.at(x);
            m_spot3D[currentKey].append(currentSpot);
        }
    }

    // Print to check
    QHash<float, QList<Spot3DCoordinate> >::iterator j;
    for (j = m_spot3D.begin(); j != m_spot3D.end(); ++j)
    {
        QList<Spot3DCoordinate> currentList = j.value();
        qDebug() << j.key() << ":"
                 << "First spot:" << "(" << currentList.at(0).x << ","
                 << currentList.at(0).y << ","
                 << currentList.at(0).z << ")";
        qDebug() << "Size:" << currentList.size();
    }
}

void Client::receiveCommand()
{
    QDataStream in(m_receiveSocket);
    in.setVersion(QDataStream::Qt_4_6);

    qCDebug(CLIENT()) << CLIENT().categoryName() << "Receiving command...";

    qint64 command;
    in >> command;
    qDebug() << command;

//  TODO
//  Refactor the following codes.
//  Don't use the command

    switch (command) {
    case START:        
        qCDebug(CLIENT()) << CLIENT().categoryName() << "RECEIVED COMMAND START.";
        emit commandStart();
        break;
    case STOP:        
        qCDebug(CLIENT()) << CLIENT().categoryName() << "RECEIVED COMMAND STOP.";
        emit commandStop();
        break;
    case PAUSE:        
        qCDebug(CLIENT()) << CLIENT().categoryName() << "RECEIVED COMMAND PAUSE.";
        emit commandPause();
        break;
    case RESUME:        
        qCDebug(CLIENT()) << CLIENT().categoryName() << "RECEIVED COMMAND RESUME.";
        emit commandResume();
        break;
    default:
        break;
    }

    m_receiveSocket->close();
    qDebug() << "Receive command finished.";
    qDebug() << SEPERATOR;
}

// Check whether the send-back data is right, not necessary
void Client::bytes(qint64 bytesWritten)
{
    qDebug() << "Bytes Written:" << bytesWritten;
}

void Client::connectServer()
{
    QHostAddress ipAddress(m_sendIpAddress);    // Set the IP address of another computer
    m_sendSocket->connectToHost(ipAddress, m_sendPort);    // Connect

    connect(m_sendSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));
}

void Client::send()
{
    connectServer();
    m_baOut.clear();

    QDataStream out(&m_baOut, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    qCDebug(CLIENT()) << CLIENT().categoryName() << "Sending ...";

    out << qint64(0)
        << qint64(0)
        << m_status;

    m_totalBytes = m_baOut.size();

    qDebug() << "m_totalBytes:" << m_totalBytes;
    qDebug() << "m_status:" << m_status;

    out.device()->seek(0);
    out << qint64(STATUS) << m_totalBytes;    // Find the head of array and write the haed information

    m_sendSocket->write(m_baOut);
    m_sendSocket->close();

//    qDebug() << "Progress information send finished.";
    qCDebug(CLIENT()) << CLIENT().categoryName() << "SEND PROGRESS UPDATE FINISHED.";
//    qDebug() << "-------------------";
}
