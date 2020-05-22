#ifndef SV_TCPSERVERCLIENT_H
#define SV_TCPSERVERCLIENT_H

#include <QObject>
#include <QString>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QDataStream>
#include <QHostAddress>

#include "sv_clog.h"

namespace sv {

  namespace net {

    /*** constants ***/
    enum ResponseAwaiting {
      DontWait = -1,
      Infinite = 0
    };

    enum SocketErrors
    {
      SOCK_UNKNOWN_ERROR = -1,
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

    enum LogDataFlags {
      NoLog = 0,
      LogInData = 0x1,
      InDataAsHex = 0x2,
      LogOutData = 0x4,
      OutDataAsHex = 0x8,
    };

    enum LogMode {
      DoNotLog,
      LogAsIs,
      LogAsHex,
      LogSizeOnly
    };

    struct Response {
      QByteArray data;
      quint64 size;
      int status;
    };

    class SvTcpServer;

  }
}

class sv::net::SvTcpServer : public QObject
{
    Q_OBJECT

  private:
    
public:
    
    SvTcpServer(sv::SvAbstractLogger &logger,
                sv::net::LogMode logRequestMode  = sv::net::LogAsIs,
                sv::net::LogMode logResponseMode = sv::net::LogAsIs,
                bool showLog = true,
                QObject *parent = 0);
    
    ~SvTcpServer();
    
    void setPort(quint16 port) { _port = port; }
    
    bool startServer(quint16 port, QObject* parent = 0);
    bool startServer(QObject* parent = 0) { return startServer(_port, parent); }
    void stopServer();
    
    QString lastError() { return _lastError; }
    
    void sendToClient(QTcpSocket* pSocket, const QByteArray &data);
    void sendToAll(const QByteArray &data);
    
private:
    
    QTcpServer* _server;
    quint16 _port = 35580;
    
    bool _isRunned = false;
    
    quint16     nextBlockSize = 0;
  
    sv::SvAbstractLogger _log;

    QByteArray _last_message;
    QHostAddress _lastClientIp;
    
    QString _lastError = "";
    
    sv::net::LogMode _logRequestMode  = sv::net::LogAsIs;
    sv::net::LogMode _logResponseMode = sv::net::LogAsIs;
    
    bool _showLog = true;
    
    QMap<quint64, QTcpSocket*> _connections;
    
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


#endif // SV_TCPSERVERCLIENT_H








