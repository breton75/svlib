#ifndef SV_TCPSERVERCLIENT_H
#define SV_TCPSERVERCLIENT_H

#include <QObject>
#include <QString>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QDataStream>
#include <QHostAddress>

#include "../../svlib/sv_log.h"

/*** constants ***/
const int dontWait = -1;
const int infinite = 0;

enum SocketErrors
{
  SOCK_ERROR = -1,
  SOCKET_OK,
  SOCKET_BAD_CONNECTION,
  SOCKET_WRONG_IP,
  SOCKET_WRONG_PORT,
  SOCKET_CONNECTION_FAULT,
  SOCKET_ALREADY_CONNECTED,
  SOCKET_NOT_CONNECTED,
  SOCKET_TIMEOUT,
  SOCKET_AWAITING_CANCELED,
  SOCKET_CONNECTION_CLOSED
};

class SvTcpServer : public QObject
{
    Q_OBJECT

  private:
    
    quint16     nextBlockSize = 0;
    void sendToClient(QTcpSocket* pSocket, const QString& str);
  
  public:

    quint16 port = 35580;
    
    SvTcpServer(quint16 port = 35580,
                   bool runServer = false,
                   QObject *parent = 0,
                   bool logRequestData = true,
                   bool showRequestDataInHex = false,
                   bool logResponseData = true,
                   bool showResponseDataInHex = false,
                   bool showLog = true,
                   bool advancedStream = false,
                   int streamVersion = QDataStream::Qt_5_2);
    
    
    QTcpServer* server;
    bool isRunned = false;
    
    int startServer(quint16 port, QObject* parent = 0);
    void stopServer();

    bool advancedStream = false;
    int streamVersion = QDataStream::Qt_5_2;

    QByteArray last_message;
    QString lastClientIp;
    bool logRequestData = true;
    bool showRequestDataInHex = false;
    bool logResponseData = true;
    bool showResponseDataInHex = false;
    bool showLog = true;
    
  signals:
    void sigGotMessage();
    void sigClientDisconnected();
    
  private slots:
    void slotSocketError(QAbstractSocket::SocketError err);
  
  public slots:
    void slotNewConnection();
    void slotReadClient   ();
    void slotClientDisconnected();
    
};

class SvTcpClient : public QObject
{
    Q_OBJECT

  private:
    quint16     nextBlockSize = 0;

  public:
    quint16 port;
    QString ip;    
    
    SvTcpClient(svlog::SvLog &log,
                QString ip = "",
                quint16 port = 35580,
                QObject *parent = 0,
                bool logRequestData = true,
                bool showRequestDataInHex = false,
                bool logResponseData = true,
                bool showResponseDataInHex = false,
                bool advancedStream = false,
                int streamVersion = QDataStream::Qt_5_2);
    
    QTcpSocket* socket;
    bool connected = false;
    
    QByteArray responseData = 0;
    quint64 responseSize = 0;
    int responseStatus = -1;
    
    virtual int connectToHost();
    virtual void disconnectFromHost();
    
    bool advancedStream = false;
    int streamVersion = QDataStream::Qt_5_2;
//    bool showSymbols = true;
    
    bool logRequestData = true;
    bool showRequestDataInHex = false;
    bool logResponseData = true;
    bool showResponseDataInHex = false;
//    bool log_requests = true;
//    bool log_responses = true;
    
    
    virtual void sendData(QByteArray* data, int msecWaitForAnswer = dontWait, QObject *parent = 0);
    virtual void sendData(QString text, int msecWaitForAnswer = dontWait, QObject* parent = 0);
    
    void simpleSendData(QString text);
    
    /* для отладки таймера */
    virtual void sendData(int msecWaitForAnswer);
    
  signals:
    virtual void sigGotNewData();
  
  private slots:
      virtual void slotReadyRead();
      virtual void slotSocketError(QAbstractSocket::SocketError);
      virtual void slotStateChanged(QAbstractSocket::SocketState state);

};

#endif // SV_TCPSERVERCLIENT_H








