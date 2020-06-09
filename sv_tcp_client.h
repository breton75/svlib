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

namespace svtcp {

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
  
  class SvTcpClient;

}

class svtcp::SvTcpClient : public QObject
{
    Q_OBJECT

    sv::SvAbstractLogger *_logger = nullptr;

  public:
    SvTcpClient(QString ip = "", quint16 port = 35580,
                sv::SvAbstractLogger *logger = nullptr,
                int flags = svtcp::LogInData | svtcp::LogOutData,
                QObject *parent = nullptr);

    ~SvTcpClient();
    
    void setLogger(sv::SvAbstractLogger *logger) { _logger = logger; }
    
    void setIp(QString ip) { _ip = ip; }
    void setPort(quint16 port) { _port = port; }
    void setFlags(int flags) { _log_flags = flags; }
    int logFlags() { return _log_flags; }
    
    svtcp::SocketErrors connectToHost();
    void disconnectFromHost();
    
    void sendData(const QByteArray &data, int msecWaitForAnswer = svtcp::DontWait);
    void sendData(QString text, int msecWaitForAnswer = svtcp::DontWait);
    
//    void simpleSendData(QString text);
    
    bool connected() { return _connected; }
    const svtcp::Response *response() { return &_response; }
    
    QString lastError() { return _lastError; }

    QTcpSocket* socket() const { return _socket; }
    
    /* для отладки таймера */
//    void sendData(int msecWaitForAnswer);
    
private:
  QObject *_parent = nullptr;
  
  QString _ip = "";
  quint16 _port = 35580;
    
  quint16  _nextBlockSize = 0;
  
  QTcpSocket* _socket;
  bool _connected = false;
  
  svtcp::Response _response;
  
//  QByteArray _responseData = 0;
//  quint64 _responseSize = 0;
//  int _responseStatus = -1;
  

  int _log_flags = svtcp::LogInData | svtcp::LogOutData;
    
  QString _lastError = "";
  
  signals:
    void newData();
  
  private slots:
      void readyRead();
      void socketError(QAbstractSocket::SocketError);
      void stateChanged(QAbstractSocket::SocketState state);

};


#endif // SV_TCPSERVERCLIENT_H








