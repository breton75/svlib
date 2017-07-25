#include "sv_tcpserverclient.h"
//#include "../../Common/log.h"
#include "sv_secondmeter.h"

#include <QApplication>
#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QMessageBox>

//using namespace log_ns;

QString char2hex(char c, bool toUpper = true, bool add0x = false, bool addBlank = true);


QString char2hex(char c, bool toUpper, bool add0x, bool addBlank)
{
    QString s = QString("%1").arg(uint(c), 2, 16, QLatin1Char('0'));
  
    return (add0x ? "0x" : "") + (toUpper ? s.toUpper() : s) + (addBlank ? " " : "");
}

/*******************************************************
 *                    SvTcpServer                      *
 * *****************************************************/

svtcp::SvTcpServer::SvTcpServer(svlog::SvLog &log,
                         quint16 port,
                         bool runServer,
                         QObject *parent,
                         bool logRequestData,
                         bool showRequestDataInHex,
                         bool logResponseData,
                         bool showResponseDataInHex,
                         bool showLog,
                         bool advancedStream,
                         int streamVersion) :
  QObject(parent)
{
  _log = log;
  
  this->port = port;
  this->logRequestData = logRequestData;
  this->showRequestDataInHex = showRequestDataInHex;
  this->logResponseData = logResponseData;
  this->showResponseDataInHex = showResponseDataInHex;
  this->showLog = showLog;
  
  this->advancedStream = advancedStream;
  this->streamVersion = streamVersion;
  
  if(runServer && (this->port != 0))
    this->startServer(this->port, parent);
 
}

void svtcp::SvTcpServer::slotNewConnection()
{
  QTcpSocket* pClientSocket = this->server->nextPendingConnection();
  connect(pClientSocket, SIGNAL(disconnected()), this, SLOT(slotClientDisconnected()));
  connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));
  
  if(showLog)
    _log << svlog::Time << svlog::Info
         << QString("Connected client: %1").arg(pClientSocket->peerAddress().toString())
         << svlog::endl;
  
  //this->sendToClient(pClientSocket, "Connected to " + pClientSocket->localAddress().toString());
}

void svtcp::SvTcpServer::slotClientDisconnected()
{
  QTcpSocket* pClientSocket = (QTcpSocket*)sender();
  if(showLog) 
    _log << svlog::Time << svlog::Info
         << QString("Client %1 disconnected").arg(pClientSocket->peerAddress().toString())
         << svlog::endl;
  
  pClientSocket->close();
  pClientSocket->deleteLater();
  
  emit sigClientDisconnected();
}

void svtcp::SvTcpServer::sendToClient(QTcpSocket* pSocket, const QString& str)
{
  QByteArray  arrBlock;
  QString s = str;

  arrBlock.append(s);
  
  pSocket->write(arrBlock);
  
  /******** выводим в лог *********/
  if(logResponseData)
  {
    if(showResponseDataInHex)
    {
      s = "";
      for (int i=0; i < arrBlock.size(); i++)
        s = s + char2hex(arrBlock.at(i));
    }
    
    if(showLog)
      _log << svlog::Time << svlog::Info << s << svlog::endl;
    
  }
  else if(showLog)
    _log << svlog::Time << svlog::Info
         << QString(" << %1 bytes sent").arg(arrBlock.size())
         << svlog::endl;
}

void svtcp::SvTcpServer::slotReadClient()
{
  QTcpSocket* pClientSocket = (QTcpSocket*)sender();
  QString message = "";
  
  this->last_message.clear();
  
  if(pClientSocket->bytesAvailable() > 0)
    this->last_message = pClientSocket->readAll();
  
  this->lastClientIp = pClientSocket->peerAddress().toString();
  
  /******** выводим в лог *********/
  if(this->logRequestData & showLog)
  {
    message = "";
    if(this->showRequestDataInHex)
    {
      for (int i=0; i < this->last_message.size(); i++)
        message = message + char2hex(this->last_message.at(i));
    }
    else message.append(this->last_message);
    
    _log << svlog::Time << svlog::Info
         << message << svlog::endl;
  }
  
  /********** отправляем клиенту OK ************/
//  if(this->last_message.toUpper().contains(QString("ERROR").toLatin1()))
//  { 
//    pClientSocket->close();
//    pClientSocket->deleteLater();
//  }
//  else
    this->sendToClient(pClientSocket, "OK");
    
  emit this->sigGotMessage();
  
}

int svtcp::SvTcpServer::startServer(quint16 port, QObject* parent)
{
  if(this->isRunned)
  {
    if(showLog)
      _log << svlog::Time << svlog::Error
           << "Already runned" << svlog::endl;
    
    return 0;
  }
  
  this->server = new QTcpServer(parent);
  this->port = port;
  
  if(!this->server->listen(QHostAddress::Any, this->port))
  {
    _log << svlog::Time << svlog::Error << "Server not runned" << svlog::endl;
    
    this->server->close();
    return 1;
  }
  
  connect(this->server, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(slotSocketError(QAbstractSocket::SocketError)));
  connect(this->server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));

  if(showLog) 
    _log << svlog::Time << svlog::Info << "Server runned" << svlog::endl;
  
  return 0;
}

void svtcp::SvTcpServer::stopServer()
{
  while(this->server->hasPendingConnections())
  {
    QTcpSocket* connection = this->server->nextPendingConnection();
    connection->close();
    connection->deleteLater();
  }
  
  this->server->close();
  this->isRunned = false;
  if(showLog) 
    _log << svlog::Time << svlog::Info << "Server stopped" << svlog::endl;

}

void svtcp::SvTcpServer::slotSocketError(QAbstractSocket::SocketError err)
{
  switch (err)
  {
    case QAbstractSocket::ConnectionRefusedError:
      _log << svlog::Time << svlog::Error << "Connection Refused Error" << svlog::endl;
      break;

    case QAbstractSocket::RemoteHostClosedError :
      _log << svlog::Time << svlog::Error << QString("Remote Host Closed Error") << svlog::endl;
      break;

    case QAbstractSocket::HostNotFoundError :
      _log << svlog::Time << svlog::Error << QString("Host Not Found Error") << svlog::endl;
      break;

    case QAbstractSocket::SocketAccessError :
      _log << svlog::Time << svlog::Error << QString("Socket Access Error") << svlog::endl;
      break;

    case QAbstractSocket::SocketResourceError :
      _log << svlog::Time << svlog::Error << QString("Socket Resource Error") << svlog::endl;
      break;

    case QAbstractSocket::SocketTimeoutError :
      _log << svlog::Time << svlog::Error << QString("Socket Timeout Error") << svlog::endl;
      break;

    case QAbstractSocket::DatagramTooLargeError :
     _log << svlog::Time << svlog::Error <<  QString("DatagramTooLargeError") << svlog::endl;
      break;

    case QAbstractSocket::NetworkError :
      _log << svlog::Time << svlog::Error << QString("Network Error") << svlog::endl;
      break;

    case QAbstractSocket::AddressInUseError :
      _log << svlog::Time << svlog::Error << QString("Address In Use Error") << svlog::endl;
      break;

    case QAbstractSocket::SocketAddressNotAvailableError :
      _log << svlog::Time << svlog::Error << QString("Socket Address Not Available Error") << svlog::endl;
      break;

    case QAbstractSocket::UnsupportedSocketOperationError :
      _log << svlog::Time << svlog::Error << QString("Unsupported Socket Operation Error") << svlog::endl;
      break;

    case QAbstractSocket::UnfinishedSocketOperationError :
      _log << svlog::Time << svlog::Error << QString("Unfinished Socket Operation Error") << svlog::endl;
      break;

    case QAbstractSocket::ProxyAuthenticationRequiredError :
      _log << svlog::Time << svlog::Error << QString("Proxy Authentication Required Error") << svlog::endl;
      break;

    case QAbstractSocket::SslHandshakeFailedError :
      _log << svlog::Time << svlog::Error << QString("Ssl Handshake Failed Error") << svlog::endl;
      break;

    case QAbstractSocket::ProxyConnectionRefusedError :
      _log << svlog::Time << svlog::Error << QString("Proxy Connection Refused Error") << svlog::endl;
      break;

    case QAbstractSocket::ProxyConnectionClosedError :
      _log << svlog::Time << svlog::Error << QString("Proxy Connection Closed Error") << svlog::endl;
      break;

    case QAbstractSocket::ProxyConnectionTimeoutError :
      _log << svlog::Time << svlog::Error << QString("Proxy Connection Timeout Error") << svlog::endl;
      break;

    case QAbstractSocket::ProxyNotFoundError :
      _log << svlog::Time << svlog::Error << QString("Proxy Not Found Error") << svlog::endl;
      break;

    case QAbstractSocket::ProxyProtocolError :
      _log << svlog::Time << svlog::Error << QString("Proxy Protocol Error") << svlog::endl;
      break;

    case QAbstractSocket::OperationError :
      _log << svlog::Time << svlog::Error << QString("Operation Error") << svlog::endl;
      break;

    case QAbstractSocket::SslInternalError :
      _log << svlog::Time << svlog::Error << QString("SslInternal Error") << svlog::endl;
      break;

    case QAbstractSocket::SslInvalidUserDataError :
      _log << svlog::Time << svlog::Error << QString("Ssl Invalid User Data Error") << svlog::endl;
      break;

    case QAbstractSocket::TemporaryError :
      _log << svlog::Time << svlog::Error << QString("Temporary Error") << svlog::endl;
      break;

    case QAbstractSocket::UnknownSocketError :
      _log << svlog::Time << svlog::Error << QString("Unknown Socket Error") << svlog::endl;
      break;

  }

}

/*******************************************************
 *                      SvTcpClient                    *
 * *****************************************************/

svtcp::SvTcpClient::SvTcpClient(QString ip, quint16 port,
                                QTextEdit *logWidget,
                                int flags,
                                QObject *parent) : 
  QObject(parent)
{
  _parent = parent;
  
  _ip = ip;
  _port = port;
  _log = svlog::SvLog(logWidget, _parent);
  _log_flags = flags;
  
  _socket = new QTcpSocket(this);
  connect(_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
  connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
  connect(_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChanged(QAbstractSocket::SocketState)));
   
}

svtcp::SocketErrors svtcp::SvTcpClient::connectToHost()
{
  if (QHostAddress(_ip).toIPv4Address() == 0) {
      _lastError = "Wrong IP";
      socketError(QAbstractSocket::HostNotFoundError);
      return svtcp::SOCKET_WRONG_IP;
  }
      
  if((_port < 1) || (_port > 65535)) {
    _lastError = "Wrong port";
    socketError(QAbstractSocket::HostNotFoundError);
    return svtcp::SOCKET_WRONG_PORT;
  }
  
  if (!_connected) {
    
      _socket->connectToHost(_ip, _port);
      
      if (_socket->waitForConnected(20000))
        
          _connected = true;
      
      else {
        _lastError = _socket->errorString();
        return svtcp::SOCKET_BAD_CONNECTION;
      }
  }
  
  return svtcp::SOCKET_OK;
  
}

void svtcp::SvTcpClient::disconnectFromHost()
{
  _socket->disconnectFromHost();
  _socket->close();
  _connected  = false;
}

void svtcp::SvTcpClient::socketError(QAbstractSocket::SocketError err)
{
  if(!_socket->errorString().isEmpty())
    _lastError = _socket->errorString();
  
  _log << svlog::Time << svlog::Error << _lastError << svlog::endl;
  
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

void svtcp::SvTcpClient::stateChanged(QAbstractSocket::SocketState state)
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
  
  _log << svlog::Time << svlog::Info << stateString << svlog::endl;
  QApplication::processEvents();
  
}

void svtcp::SvTcpClient::readyRead()
{
  if (_socket->bytesAvailable() > 0)
  {
    _response.data = _socket->readAll();
    _response.size = _response.data.size();
    
    /***** выводим лог *****/
    if(_log_flags) {
      
      if(_log_flags & svtcp::LogInData) {
        
        QString str;
        if(_log_flags & svtcp::InDataAsHex) {
          
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
        
        _log << svlog::Time << svlog::Data << svlog::in << str << svlog::endl;
      }
      
      else
        _log << svlog::Time << svlog::Data << svlog::in 
             << QString("%1 bytes got").arg(_response.size) << svlog::endl;
    }
    
    emit newData();
  }
    
}

void svtcp::SvTcpClient::sendData(QString text, int msecWaitForAnswer)
{
  if(!_socket->isOpen()) return;
  
  SvSecondMeter *t = nullptr;

  _response.size = 0;
  _response.data.clear();
  
  /***** готовимся ждать ответ *****/
  if(msecWaitForAnswer != svtcp::DontWait)
  {
    t = new SvSecondMeter(msecWaitForAnswer, "Waiting for response", "Cancel", false, true, _parent);
    connect(this, SIGNAL(newData()), t, SLOT(slotDone()));
  }
  
  /***** отправляем данные *****/
  _socket->write(QByteArray::fromStdString(text.toStdString()));
  
  /***** выводим лог *****/
  if(_log_flags) {
    if(_log_flags & svtcp::LogOutData)
      _log << svlog::Time << svlog::Data << svlog::out << text << svlog::endl;

    else
      _log << svlog::Time << svlog::Data << svlog::out << QString(" << %1 bytes sent").arg(text.length());
  }
  
  if(!t) return;
  
  /* запускаем таймер ожидания ответа */
  t->start();
  
  /*********************** ждем ответ ***********************/
  while((t->status == SvSecondMeter::smsRunned) && (_socket->isOpen()))
    QApplication::processEvents();
  
  if(!_socket->isOpen()) t->status = SvSecondMeter::smsUnfinished;
  
  switch (t->status)
  {
    case SvSecondMeter::smsCanceled:
      _socket->abort();
      _log << svlog::Time << svlog::Info
           << QString("Response awaiting was canceled by user at %1")
           << t->current_time
           << svlog::endl;
      
      _response.status = SOCKET_AWAITING_CANCELED;
      break;
      
    case SvSecondMeter::smsTimeout:
      _socket->abort();
      
      _log << svlog::Time << svlog::Error
           << QString("Response awaiting timeout %1")
           << t->current_time
           << svlog::endl;
      
      _response.status = SOCKET_TIMEOUT;
      break;
      
    case SvSecondMeter::smsUnfinished:
      _log << svlog::Time << svlog::Error
           <<  QString("Awaiting not finished %1")
           << t->current_time
           << svlog::endl;
      
      _response.status = SOCKET_CONNECTION_CLOSED;
      break;
      
    case SvSecondMeter::smsDoneOk:
      _response.status = SOCKET_OK;      
  }
  
  
  t->~SvSecondMeter();
  
}
 
void svtcp::SvTcpClient::sendData(const QByteArray &data, int msecWaitForAnswer)
{
    if(!_socket->isOpen()) return;
  
    SvSecondMeter *t = nullptr;
    
    /***** готовимся ждать ответ *****/
    if(msecWaitForAnswer != svtcp::DontWait) {
      
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
      if(_log_flags & svtcp::LogOutData) {
        
        if(_log_flags & svtcp::OutDataAsHex) {
          
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
      
      _log << svlog::Time << svlog::Data << svlog::out << str << svlog::endl;
      
    }
      
    if(!t) return;
    
    t->start();
    
    /*********************** ждем ответ ***********************/
    while((t->status == SvSecondMeter::smsRunned)&&(_socket->isOpen())) QApplication::processEvents();
    if(!_socket->isOpen()) t->status = SvSecondMeter::smsUnfinished;
    
    switch (t->status)
    {
      case SvSecondMeter::smsCanceled:
        _socket->abort();
        _log << svlog::Time << svlog::Info
             << QString("Response awaiting was canceled by user at %1")
             << t->current_time
             << svlog::endl;
        
        _response.status = SOCKET_AWAITING_CANCELED;
        break;
        
      case SvSecondMeter::smsTimeout:
        _socket->abort();
        
        _log << svlog::Time << svlog::Error
             << QString("Response awaiting timeout %1")
             << t->current_time
             << svlog::endl;
        
        _response.status = SOCKET_TIMEOUT;
        break;
        
      case SvSecondMeter::smsUnfinished:
        _log << svlog::Time << svlog::Error
             <<  QString("Awaiting not finished %1")
             << t->current_time
             << svlog::endl;
        
        _response.status = SOCKET_CONNECTION_CLOSED;
        break;
        
      case SvSecondMeter::smsDoneOk:
        _response.status = SOCKET_OK;      
    }
    
    t->~SvSecondMeter();
}
 
void svtcp::SvTcpClient::sendData(int msecWaitForAnswer)
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
      _log << svlog::Time << svlog::Info
           << QString("Response awaiting was canceled by user at %1")
           << t->current_time
           << svlog::endl;
      
      _response.status = SOCKET_AWAITING_CANCELED;
      break;
      
    case SvSecondMeter::smsTimeout:
      _log << svlog::Time << svlog::Error
           << QString("Response awaiting timeout %1")
           << t->current_time
           << svlog::endl;
      
      _response.status = SOCKET_TIMEOUT;
      break;
      
    case SvSecondMeter::smsUnfinished:
      _log << svlog::Time << svlog::Error
           <<  QString("Awaiting not finished %1")
           << t->current_time
           << svlog::endl;
      
      _response.status = SOCKET_CONNECTION_CLOSED;
      break;
      
    case SvSecondMeter::smsDoneOk:
      _response.status = SOCKET_OK;      
  }
  
  t->~SvSecondMeter();
}

//void svtcp::SvTcpClient::simpleSendData(QString text)
//{
//  if(!this->socket->isOpen()) return;
  
//  QByteArray arrBlock;

//  /***** отправляем данные *****/
//  arrBlock.append(text);
//  arrBlock.append('\n');
//  this->socket->write(arrBlock);
  
//  /***** выводим лог *****/
//  if(this->logRequestData)
//  {
//    log(m_Data, " << " + text);
//  }
//  else log(m_Data, QString(" << %1 bytes sent").arg(text.length()));

  
//}
