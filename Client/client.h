#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QtNetwork>
#include <QHash>
#include <QVariant>
#include <QLoggingCategory>

#include "variable.h"
#include "constant.h"
#include "client_global.h"

Q_DECLARE_LOGGING_CATEGORY(CLIENT)

class CLIENTSHARED_EXPORT Client : public QObject
{
    Q_OBJECT

public:
    Client(QObject *parent = 0);
    ~Client();

    inline void setStatus(QHash<QString, QVariant> status) { m_status = status; }
    void send();

public slots:
    void listen();    // Start to listen port    

    inline QHash<float, QList<Spot3DCoordinate> > getCoordinate(){ return m_spot3D; }
    inline QHash<float, QList<int> > getSpotOrder(){ return m_spotOrder; }
    inline SpotSonicationParameter getParameter(){ return m_parameter; }

signals:
    commandStart();
    commandStop();
    commandPause();
    commandResume();
    receivingCompleted();

private slots:
    void acceptConnection();    // Build connection
    void displayError(QAbstractSocket::SocketError socketError);    //Display the error
    QString getLocalIP();
    void initVar();

    void convertSpot();
    void readHeader();
    void receivePlan();
    void receiveCommand();
    void bytes(qint64 bytesWritten);

    void connectServer();

private:
    QTcpServer m_server;
    QTcpSocket *m_receiveSocket, *m_sendSocket;
    QString m_receiveIpAddress, m_sendIpAddress;
    quint16 m_receivePort, m_sendPort;
    void readSettings();
    void updateSettings();

    QByteArray m_baOut;    // Data buffer for write
    qint64 m_totalBytes;    // Total bytes of data to send or receive
    QHash<float, QList<Spot3DCoordinate> > m_spot3D;    // Save spot coordinates data
    QHash<float, QList<Coordinate> > m_hashX, m_hashY, m_hashZ;
    QHash<float, QList<int> > m_spotOrder;
    SpotSonicationParameter m_parameter;

    QHash<QString, QVariant> m_status;
};

#endif // CLIENT_H
