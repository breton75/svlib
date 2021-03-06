#include "sv_tcp_server.h"
#include "sv_secondmeter.h"

#include <QByteArray>
#include <QDataStream>
#include <QDebug>

QString char2hex(char c, bool toUpper = true, bool add0x = false, bool addBlank = true);


QString char2hex(char c, bool toUpper, bool add0x, bool addBlank)
{
    QString s = QString("%1").arg(uint(c), 2, 16, QLatin1Char('0'));
  
    return (add0x ? "0x" : "") + (toUpper ? s.toUpper() : s) + (addBlank ? " " : "");
}

/*******************************************************
 *                    SvTcpServer                      *
 * *****************************************************/

sv::net::SvTcpServer::SvTcpServer(sv::SvAbstractLogger &logger,
                         LogMode logRequestMode,
                         LogMode logResponseMode,
                         bool showLog,
                         QObject *parent) :
  QObject(parent)
{
  _log = logger;
  
  _logRequestMode = logRequestMode;
  _logResponseMode = logResponseMode;
  _showLog = showLog;
 
}

sv::net::SvTcpServer::~SvTcpServer()
{
  stopServer();
  deleteLater();
}

void sv::net::SvTcpServer::slotNewConnection()
{
  QTcpSocket* pClientSocket = _server->nextPendingConnection();
  
  _connections.insert(quint64(pClientSocket)/*->socketDescriptor()*/, pClientSocket);
  
  connect(pClientSocket, SIGNAL(disconnected()), this, SLOT(slotClientDisconnected()));
  connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));
  
  if(_showLog)
    _log << sv::log::Time << sv::log::mtInfo
         << QString("Connected client: %1").arg(pClientSocket->peerAddress().toString())
         << sv::log::endl;
  
  //this->sendToClient(pClientSocket, "Connected to " + pClientSocket->localAddress().toString());
}

void sv::net::SvTcpServer::slotClientDisconnected()
{
  QTcpSocket* pClientSocket = (QTcpSocket*)sender();
  
  if(_showLog) 
    _log << sv::log::Time << sv::log::mtInfo
         << QString("Client %1 disconnected").arg(pClientSocket->peerAddress().toString())
         << sv::log::endl;
  
  _connections.remove(quint64(pClientSocket));

  pClientSocket->close();
  
  emit sigClientDisconnected();
}

void sv::net::SvTcpServer::sendToClient(QTcpSocket* pSocket, const QByteArray& data)
{
  
  pSocket->write(data);
  
  /******** выводим в лог *********/
  if(_logResponseMode != sv::net::DoNotLog) {
    
    QString s = QString(data);
    
    switch (_logResponseMode) {
      
      case sv::net::LogAsIs:
        break;
        
      case sv::net::LogAsHex: {
        
        s = "";
        for (int i=0; i < data.size(); i++)
          s += char2hex(data.at(i));
        
        break;
      }
        
      case sv::net::LogSizeOnly:
        s =  QString("%1 bytes sent").arg(data.size());
        break;
        
    }
    
    _log << sv::log::Time << sv::log::mtInfo << QString(" << %1").arg(s) << sv::log::endl;
      
  }
  
}

void sv::net::SvTcpServer::sendToAll(const QByteArray& data)
{
  for(QTcpSocket* client: _connections.values()) {

    sendToClient(client, data);
//    qDebug() << data.size() << client->localPort() << client->peerPort() << client->peerAddress().toString();
  }
}

void sv::net::SvTcpServer::slotReadClient()
{
  QTcpSocket* pClientSocket = (QTcpSocket*)sender();
  
  _last_message.clear();
  
  if(pClientSocket->bytesAvailable() > 0)
    _last_message = pClientSocket->readAll();
  
  _lastClientIp = pClientSocket->peerAddress();
  
  /******** выводим в лог *********/
  if(_showLog) {
    
    QString s = _last_message;
    
    switch (_logResponseMode) {
      
      case sv::net::LogAsIs:
        break;
        
      case sv::net::LogAsHex: {
        
        s = "";
        for (int i = 0; i < _last_message.size(); i++)
          s += char2hex(_last_message.at(i));
        
        break;
      }
        
      case sv::net::LogSizeOnly:
        s =  QString(" << %1 bytes got").arg(_last_message.size());
        break;
        
    }
    
    _log << sv::log::Time << sv::log::mtInfo << s << sv::log::endl;
      
  }
  
  sendToClient(pClientSocket, "OK");
    
  emit sigGotMessage();
  
}

bool sv::net::SvTcpServer::startServer(quint16 port, QObject* parent)
{
  if(_isRunned)
    return true;
  
  _server = new QTcpServer(parent);
  _port = port;
  
  if(!_server->listen(QHostAddress::Any, _port)) {
    
    _lastError = QString("%1").arg(_server->errorString());
    _server->close();
    
    return false;
  }
  
  connect(_server, QTcpServer::acceptError, this, sv::net::SvTcpServer::slotSocketError);
  connect(_server, QTcpServer::newConnection, this, sv::net::SvTcpServer::slotNewConnection);

  if(_showLog) 
    _log << sv::log::Time << sv::log::mtInfo << QString("TCP server runned (%1)").arg(_port) << sv::log::endl;
  
  _isRunned = true;
      
  return true;
  
}

void sv::net::SvTcpServer::stopServer()
{
  _server->close();
  delete _server;
  
  _isRunned = false;
  
  if(_showLog) 
    _log << sv::log::Time << sv::log::mtInfo << QString("TCP server stopped (%1)").arg(_port) << sv::log::endl;

}

void sv::net::SvTcpServer::slotSocketError(QAbstractSocket::SocketError err)
{
  switch (err) {
    
    case QAbstractSocket::ConnectionRefusedError:
      _log << sv::log::Time << sv::log::mtError << "Connection Refused Error" << sv::log::endl;
      break;

    case QAbstractSocket::RemoteHostClosedError :
      _log << sv::log::Time << sv::log::mtError << QString("Remote Host Closed Error") << sv::log::endl;
      break;

    case QAbstractSocket::HostNotFoundError :
      _log << sv::log::Time << sv::log::mtError << QString("Host Not Found Error") << sv::log::endl;
      break;

    case QAbstractSocket::SocketAccessError :
      _log << sv::log::Time << sv::log::mtError << QString("Socket Access Error") << sv::log::endl;
      break;

    case QAbstractSocket::SocketResourceError :
      _log << sv::log::Time << sv::log::mtError << QString("Socket Resource Error") << sv::log::endl;
      break;

    case QAbstractSocket::SocketTimeoutError :
      _log << sv::log::Time << sv::log::mtError << QString("Socket Timeout Error") << sv::log::endl;
      break;

    case QAbstractSocket::DatagramTooLargeError :
     _log << sv::log::Time << sv::log::mtError <<  QString("DatagramTooLargeError") << sv::log::endl;
      break;

    case QAbstractSocket::NetworkError :
      _log << sv::log::Time << sv::log::mtError << QString("Network Error") << sv::log::endl;
      break;

    case QAbstractSocket::AddressInUseError :
      _log << sv::log::Time << sv::log::mtError << QString("Address In Use Error") << sv::log::endl;
      break;

    case QAbstractSocket::SocketAddressNotAvailableError :
      _log << sv::log::Time << sv::log::mtError << QString("Socket Address Not Available Error") << sv::log::endl;
      break;

    case QAbstractSocket::UnsupportedSocketOperationError :
      _log << sv::log::Time << sv::log::mtError << QString("Unsupported Socket Operation Error") << sv::log::endl;
      break;

    case QAbstractSocket::UnfinishedSocketOperationError :
      _log << sv::log::Time << sv::log::mtError << QString("Unfinished Socket Operation Error") << sv::log::endl;
      break;

    case QAbstractSocket::ProxyAuthenticationRequiredError :
      _log << sv::log::Time << sv::log::mtError << QString("Proxy Authentication Required Error") << sv::log::endl;
      break;

    case QAbstractSocket::SslHandshakeFailedError :
      _log << sv::log::Time << sv::log::mtError << QString("Ssl Handshake Failed Error") << sv::log::endl;
      break;

    case QAbstractSocket::ProxyConnectionRefusedError :
      _log << sv::log::Time << sv::log::mtError << QString("Proxy Connection Refused Error") << sv::log::endl;
      break;

    case QAbstractSocket::ProxyConnectionClosedError :
      _log << sv::log::Time << sv::log::mtError << QString("Proxy Connection Closed Error") << sv::log::endl;
      break;

    case QAbstractSocket::ProxyConnectionTimeoutError :
      _log << sv::log::Time << sv::log::mtError << QString("Proxy Connection Timeout Error") << sv::log::endl;
      break;

    case QAbstractSocket::ProxyNotFoundError :
      _log << sv::log::Time << sv::log::mtError << QString("Proxy Not Found Error") << sv::log::endl;
      break;

    case QAbstractSocket::ProxyProtocolError :
      _log << sv::log::Time << sv::log::mtError << QString("Proxy Protocol Error") << sv::log::endl;
      break;

    case QAbstractSocket::OperationError :
      _log << sv::log::Time << sv::log::mtError << QString("Operation Error") << sv::log::endl;
      break;

    case QAbstractSocket::SslInternalError :
      _log << sv::log::Time << sv::log::mtError << QString("SslInternal Error") << sv::log::endl;
      break;

    case QAbstractSocket::SslInvalidUserDataError :
      _log << sv::log::Time << sv::log::mtError << QString("Ssl Invalid User Data Error") << sv::log::endl;
      break;

    case QAbstractSocket::TemporaryError :
      _log << sv::log::Time << sv::log::mtError << QString("Temporary Error") << sv::log::endl;
      break;

    case QAbstractSocket::UnknownSocketError :
      _log << sv::log::Time << sv::log::mtError << QString("Unknown Socket Error") << sv::log::endl;
      break;

  }

}

/*******************************************************
 *                      SvTcpClient                    *
 * *****************************************************/

sv::net::SvTcpClient::SvTcpClient(QString ip, quint16 port,
                                QTextEdit *logWidget,
                                int flags,
                                QObject *parent) : 
  QObject(parent)
{
  _parent = parent;
  
  _ip = ip;
  _port = port;
//  _log = sv::log::SvLog(logWidget, _parent);
  _log.setTextEdit(logWidget);
  _log_flags = flags;
  
  _socket = new QTcpSocket(this);
  connect(_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
  connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
  connect(_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChanged(QAbstractSocket::SocketState)));
   
}

sv::net::SocketErrors sv::net::SvTcpClient::connectToHost()
{
  if (QHostAddress(_ip).toIPv4Address() == 0) {
      _lastError = "Wrong IP";
      socketError(QAbstractSocket::HostNotFoundError);
      return sv::net::SOCKET_WRONG_IP;
  }
      
  if((_port < 1) || (_port > 65535)) {
    _lastError = "Wrong port";
    socketError(QAbstractSocket::HostNotFoundError);
    return sv::net::SOCKET_WRONG_PORT;
  }
  
  if (!_connected) {
    
      _socket->connectToHost(_ip, _port);
      
      if (_socket->waitForConnected(20000))
        
          _connected = true;
      
      else {
        _lastError = _socket->errorString();
        return sv::net::SOCKET_BAD_CONNECTION;
      }
  }
  
  return sv::net::SOCKET_OK;
  
}

void sv::net::SvTcpClient::disconnectFromHost()
{
  _socket->disconnectFromHost();
  _socket->close();
  _connected  = false;
}

void sv::net::SvTcpClient::socketError(QAbstractSocket::SocketError err)
{
  if(!_socket->errorString().isEmpty())
    _lastError = _socket->errorString();
  
  _log << sv::log::Time << sv::log::mtError << _lastError << sv::log::endl;
  
  switch (err)
  {
    case QAbstractSocket::ConnectionRefusedError:
//      log(m_Err, "Connection Refused Error");
      _connected = false;
      QApplication::processEvents();
      break;
      
    case QAbstractSocket::RemoteHostClosedError :
//      log(m_Err, QString("Remote Host Closed Error"));
      QApplication::processEvents();
      break;   
      
    case QAbstractSocket::HostNotFoundError :
//      log(m_Err, QString("Host Not Found Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::SocketAccessError :
//      log(m_Err, QString("Socket Access Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::SocketResourceError :
//      log(m_Err, QString("Socket Resource Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::SocketTimeoutError :
//      log(m_Err, QString("Socket Timeout Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::DatagramTooLargeError :
//      log(m_Err, QString("DatagramTooLargeError"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::NetworkError :
//      log(m_Err, QString("Network Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::AddressInUseError :
//      log(m_Err, QString("Address In Use Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::SocketAddressNotAvailableError :
//      log(m_Err, QString("Socket Address Not Available Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::UnsupportedSocketOperationError :
//      log(m_Err, QString("Unsupported Socket Operation Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::UnfinishedSocketOperationError :
//      log(m_Err, QString("Unfinished Socket Operation Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::ProxyAuthenticationRequiredError :
//      log(m_Err, QString("Proxy Authentication Required Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::SslHandshakeFailedError :
//      log(m_Err, QString("Ssl Handshake Failed Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::ProxyConnectionRefusedError :
//      log(m_Err, QString("Proxy Connection Refused Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::ProxyConnectionClosedError :
//      log(m_Err, QString("Proxy Connection Closed Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::ProxyConnectionTimeoutError :
//      log(m_Err, QString("Proxy Connection Timeout Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::ProxyNotFoundError :
//      log(m_Err, QString("Proxy Not Found Error"));
      QApplication::processEvents();
      break;
      
    case QAbstractSocket::ProxyProtocolError :  
//      log(m_Err, QString("Proxy Protocol Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::OperationError :
//      log(m_Err, QString("Operation Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::SslInternalError :
//      log(m_Err, QString("SslInternal Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::SslInvalidUserDataError :
//      log(m_Err, QString("Ssl Invalid User Data Error"));
      QApplication::processEvents();
      break;
      
    case QAbstractSocket::TemporaryError :
//      log(m_Err, QString("Temporary Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::UnknownSocketError :
//      log(m_Err, QString("Unknown Socket Error"));
      QApplication::processEvents();
      break;

  }
  
  
//  QString strError =
//        "Error: " + (err == QAbstractSocket::HostNotFoundError ?
//                     "The host was not found." :
//                     err == QAbstractSocket::RemoteHostClosedError ?
//                     "The remote host is closed." :
//                     err == QAbstractSocket::ConnectionRefusedError ?
//                     "The connection was refused." :
//                     (QString(this->socket->errorString()) != "" ? QString(this->socket->errorString()) :
//                      QString(this->socket->errorString()))
//                    );
//    log(m_Err, strError);
}

void sv::net::SvTcpClient::stateChanged(QAbstractSocket::SocketState state)
{
  QString stateString;
  
  switch (state)
    {
    case QAbstractSocket::UnconnectedState:
      stateString = "No connection";
      _connected = false;
      break;
      
    case QAbstractSocket::HostLookupState:
      stateString = QString("Looking up for %1").arg(_ip);
      break;   

    case QAbstractSocket::ConnectingState:
      stateString = QString("Connecting to %1").arg(_ip);
      break;
      
    case QAbstractSocket::ConnectedState:
      _connected = true;
      stateString = QString("Connected to %1").arg(_ip);
      break;

    case QAbstractSocket::BoundState:
      stateString = QString("Bounded for %1:%2").arg(_ip).arg(_port);
      break; 
      
    case QAbstractSocket::ClosingState:
      stateString = "Closing connection";
      break;       
  
    default:
      stateString = "state ";// + QString("%1").arg(int(state)));
  }
  
  _log << sv::log::Time << sv::log::mtInfo << stateString << sv::log::endl;
  QApplication::processEvents();
  
}

void sv::net::SvTcpClient::readyRead()
{
  if (_socket->bytesAvailable() > 0)
  {
    _response.data = _socket->readAll();
    _response.size = _response.data.size();
    
    /***** выводим лог *****/
    if(_log_flags) {
      
      if(_log_flags & sv::net::LogInData) {
        
        QString str;
        if(_log_flags & sv::net::InDataAsHex) {
          
          for (int i = 0; i < _response.data.size(); i++) {
            
            if(i%16 == 0) str += '\n';
            else if(i%8 == 0) str += ' ';
            
            str += char2hex(_response.data.at(i), true, false, true);
            
          }
        }
        
        else {
          
          for (int i = 0; i < _response.data.size(); i++)
            str += _response.data.at(i) < 32 ? '.' : uchar(_response.data.at(i));
      
        }
        
        _log << sv::log::Time << sv::log::Data << sv::log::in << str << sv::log::endl;
      }
      
      else
        _log << sv::log::Time << sv::log::Data << sv::log::in
             << QString("%1 bytes got").arg(_response.size) << sv::log::endl;
    }
    
    emit newData();
  }
    
}

void sv::net::SvTcpClient::sendData(QString text, int msecWaitForAnswer)
{
  QByteArray ba = QByteArray();
  ba.append(text);
  
  sendData(ba, msecWaitForAnswer);
  
}
 
void sv::net::SvTcpClient::sendData(const QByteArray &data, int msecWaitForAnswer)
{
    if(!_socket->isOpen()) return;
  
    SvSecondMeter *t = nullptr;
    
    /***** готовимся ждать ответ *****/
    if(msecWaitForAnswer != sv::net::DontWait) {
      
      t = new SvSecondMeter(msecWaitForAnswer, "Waiting for response", "Cancel", false, true, _parent);
      connect(this, SIGNAL(newData()), t, SLOT(slotDone()));
      
      _response.size = 0;
      _response.data.clear();   
    }
    
    /***** отправляем данные *****/
    _socket->write(data);
    
    
    /***** выводим лог *****/
    if(_log_flags) {
    
      QString str = "";
      if(_log_flags & sv::net::LogOutData) {
        
        if(_log_flags & sv::net::OutDataAsHex) {
          
          for (int i = 0; i < data.size(); i++) {
            
            if(i%16 == 0) str += '\n';
            else if(i%8 == 0) str += ' ';
            
            str += char2hex(data.at(i), true, false, true);
            
          }
        }
        
        else {
          
          for (int i=0; i < data.size(); i++)           
             str += data.at(i) < 32 ? '.' : uchar(data.at(i));
  
        }
      }
      
      else
        str = QString("%1 bytes sent").arg(data.length());
      
      _log << sv::log::Time << sv::log::Data << sv::log::out << str << sv::log::endl;
      
    }
      
    if(!t) return;
    
    t->start();
    
    /*********************** ждем ответ ***********************/
    while((t->status == SvSecondMeter::smsRunned)&&(_socket->isOpen())) QApplication::processEvents();
    if(!_socket->isOpen()) t->status = SvSecondMeter::smsUnfinished;
    
    switch (t->status)
    {
      case SvSecondMeter::smsCanceled:
//        _socket->abort();
        
        _log << sv::log::Time << sv::log::mtInfo
             << QString("Response awaiting was canceled by user at %1").arg(QTime(t->current_time).toString("hh:mm:ss.zzz"))
             << sv::log::endl;
        
        _response.status = SOCKET_AWAITING_CANCELED;
        _lastError = "Awaiting cancelled by user.";
        break;
        
      case SvSecondMeter::smsTimeout:
//        _socket->abort();
        
        _log << sv::log::Time << sv::log::mtError
             << QString("Response awaiting timeout %1").arg(QTime(t->current_time).toString("hh:mm:ss.zzz"))
             << sv::log::endl;
        
        _response.status = SOCKET_TIMEOUT;
        _lastError = "Awaiting timeout.";
        break;
        
      case SvSecondMeter::smsUnfinished:
        _log << sv::log::Time << sv::log::mtError
             << QString("Awaiting not finished %1.").arg(QTime(t->current_time).toString("hh:mm:ss.zzz"))
             << t->current_time
             << sv::log::endl;
        
        _response.status = SOCKET_CONNECTION_CLOSED;
        _lastError = "Awaiting not finished.";
        break;
        
      case SvSecondMeter::smsDoneOk:
        _response.status = SOCKET_OK;      
        _lastError = "";
    }
    
    t->~SvSecondMeter();
}
 
void sv::net::SvTcpClient::sendData(int msecWaitForAnswer)
{
  /************* для отладки таймера ************/
  
  /*********************** ждем ответ ***********************/
  _response.size = 0;
  _response.data.clear();
  
  SvSecondMeter *t = new SvSecondMeter(msecWaitForAnswer,
                                       "Waiting for response",
                                       "Cancel",
                                       true,
                                       true);
  
//    connect(this, SIGNAL(sigGotNewData()), t, SLOT(slotDone()));
  
  while(t->status == SvSecondMeter::smsRunned) QApplication::processEvents();
  
  switch (t->status)
  {
    case SvSecondMeter::smsCanceled:
      _log << sv::log::Time << sv::log::mtInfo
           << QString("Response awaiting was canceled by user at %1")
           << t->current_time
           << sv::log::endl;
      
      _response.status = SOCKET_AWAITING_CANCELED;
      break;
      
    case SvSecondMeter::smsTimeout:
      _log << sv::log::Time << sv::log::mtError
           << QString("Response awaiting timeout %1")
           << t->current_time
           << sv::log::endl;
      
      _response.status = SOCKET_TIMEOUT;
      break;
      
    case SvSecondMeter::smsUnfinished:
      _log << sv::log::Time << sv::log::mtError
           <<  QString("Awaiting not finished %1")
           << t->current_time
           << sv::log::endl;
      
      _response.status = SOCKET_CONNECTION_CLOSED;
      break;
      
    case SvSecondMeter::smsDoneOk:
      _response.status = SOCKET_OK;      
  }
  
  t->~SvSecondMeter();
}

