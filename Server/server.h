#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QtNetwork>
#include <QList>
#include <QHash>
#include <QStringList>
#include <QVariant>
#include <QLoggingCategory>

#include "server_global.h"
#include "constant.h"
#include "variable.h"

Q_DECLARE_LOGGING_CATEGORY(SERVER)

class SERVERSHARED_EXPORT Server : public QObject
{
    Q_OBJECT

public:
    Server(QObject *parent = 0);
    ~Server();

    inline QHash<QString, QVariant> getStatus() { return m_status; }

public slots:
    inline void setCoordinate(QHash<float, QList<Spot3DCoordinate> > spot3D){ m_spot3D = spot3D; }
    inline void setSpotOrder(QHash<float, QList<int> > spotOrder){ m_spotOrder = spotOrder; }
    inline void setParameter(SpotSonicationParameter parameter){ m_parameter = parameter; }

    void sendPlan();
    void sendCommand(cmdType);
    void listen();

private slots:
    void handleError(QString errorString);
    void connectServer();    // Connect to client
    void displayError(QAbstractSocket::SocketError);    // Display error
    QString getLocalIP();

    void readReceipt();
    void writtenBytes(qint64);

    void updateSettings();
    void readSettings();

    void acceptConnection();
    void receive();

signals:
    sendingCompleted();
    error(QString errorString);
    receivingCompleted();

private:
    QTcpServer *m_server;
    QTcpSocket *m_sendSocket, *m_receiveSocket;

    QByteArray m_baOut;
    void encodePlan(QByteArray* baBlock);
    void encodeSpot();
    void encodeCmd(QByteArray* baBlock, cmdType iType);
//    void decodeStatus(QByteArray* baBlock);

    enum Error
    {
        NoError,
        ErrorSend,
        ErrorReadReceipt,
        ErrorReceive
    };

    QStringList m_errorList;
    void setErrorString();

    QStringList m_cmdList;
    void setCmdString();

    qint64 m_totalBytes, m_writtenBytes;    // Total bytes to send for this send progress

    QHash<float, QList<Spot3DCoordinate> > m_spot3D;
    QHash<float, QList<Coordinate> > m_hashX, m_hashY, m_hashZ;

    QHash<float, QList<int> > m_spotOrder;
    SpotSonicationParameter m_parameter;

    int m_sendTimeNum;
    QString m_receipt;
    void genReceipt(QString& receipt);

    QString m_receiveIpAddress, m_sendIpAddress;
    quint16 m_receivePort, m_sendPort;

    QHash<QString, QVariant> m_status;
};


#endif // SERVER_H
